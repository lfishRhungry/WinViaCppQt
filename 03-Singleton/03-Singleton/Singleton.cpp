#include "Singleton.h"

Singleton::Singleton(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(1000, 450);
	this->setWindowTitle("Singleton");
	ui.plainTextEdit->setReadOnly(true);
	ui.pushButton->setFixedWidth(120);

	// ���Exit��ť�رճ���
	connect(ui.pushButton, &QPushButton::clicked, this, &QWidget::close);
	// ���߼�
	CheckInstance();
}

void Singleton::CheckInstance() {
	// �����߽�������
	g_hBoundary = CreateBoundaryDescriptor(g_szBoundary, 0);

	// ����һ���������ع���Ա���ͨ��SID
	BYTE localAdminSID[SECURITY_MAX_SID_SIZE];
	PSID pLocalAdminSID = &localAdminSID;
	DWORD cbSID = sizeof(localAdminSID);
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid,
		nullptr,
		pLocalAdminSID,
		&cbSID)) {
		ui.plainTextEdit->appendPlainText(QString("AddSIDToBoundaryDescriptor failed: %1\r\n").arg(GetLastError()));
		return;
	}

	// �������õı��ع���Ա�û���SID��߽�����������
	// ����һ�� ֻ�е������ڹ���Ա�û������е�ʱ��
	// �ſ��Է���֮����˱߽�������������ͬһ�������ռ���ں˶���
	if (!AddSIDToBoundaryDescriptor(&g_hBoundary, pLocalAdminSID)) {
		ui.plainTextEdit->appendPlainText(QString("AddSIDToBoundaryDescriptor failed: %1\r\n").arg(GetLastError()));
		return;
	}

	// ����ֻ�б��ع���Ա����ʹ�õ������ռ�
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;
	// ��һ������ȫ��������ʽ���ַ���ת����һ����Ч�İ�ȫ�������ṹ
	// �����ȫ�������Ǹ������ռ��sa�õ� ���Ǹոձ߽���������
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
		TEXT("D:(A;;GA;;;BA)"), // ��ʾ�������İ�ȫ��������������ֻ���ڱ��ع���Ա
		SDDL_REVISION_1,
		&sa.lpSecurityDescriptor,
		nullptr)) {
		ui.plainTextEdit->appendPlainText(QString("Security Descriptor creation failed: %1\r\n").arg(GetLastError()));
		return;
	}
	// ��ʽ�������б߽�������ռ�
	g_hNamespace = CreatePrivateNamespace(&sa, g_hBoundary, g_szNamespace);
	// �ͷŸ������ռ��Ű�ȫ���������ɵĶѿռ�
	LocalFree(sa.lpSecurityDescriptor);

	// ��������ռ�Ĵ������ ������Ӧ����
	DWORD dwLastError = GetLastError();
	// ����������if-else�߼��ĵĺÿ���һ��
	// ��Ȼ����д���߼���������....
	if (g_hNamespace == nullptr) {
		if (dwLastError == ERROR_ACCESS_DENIED) {
			// ���������Ȩ�޲��� ��������ɹ���Ա������
			ui.plainTextEdit->appendPlainText("Access denied when creating the namespace.\r\n");
			ui.plainTextEdit->appendPlainText("	You must be running as Administrator.\r\n\r\n");
			return;
		}
		else if (dwLastError == ERROR_ALREADY_EXISTS) {
			// �Ѿ�������ʵ����������������ռ�
			// ��ô���Ǿʹ���
			ui.plainTextEdit->appendPlainText(QString("CreatePrivateNamespace failed: %1\r\n").arg(dwLastError));
			g_hNamespace = OpenPrivateNamespace(g_hBoundary, g_szNamespace);
			if (g_hNamespace == nullptr) {
				ui.plainTextEdit->appendPlainText(QString("	and OpenPrivateNamespace failed: %1\r\n").arg(dwLastError));
				return;
			}
			else {
				g_bNamespaceOpened = TRUE;
				ui.plainTextEdit->appendPlainText("	but OpenPrivateNamespace succeeded\r\n\r\n");
			}
		}
		else {
			ui.plainTextEdit->appendPlainText(QString("Unexpected error occured: %1\r\n").arg(dwLastError));
			return;
		}
	}


	// �����ĵ������ռ��д���һ����������������
	// ����������������ƣ�������ָ�����ռ���Ϣ��
	TCHAR szMutexName[64];
	StringCchPrintf(szMutexName, _countof(szMutexName), TEXT("%s\\%s"), g_szNamespace, TEXT("Singleton"));
	// ��������������
	g_hSingleton = CreateMutex(nullptr, FALSE, szMutexName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// �Ѿ�����һ��ʵ��
		ui.plainTextEdit->appendPlainText("Another instance of Singleton is running:\r\n");
		ui.plainTextEdit->appendPlainText("--> Impossible to access application features.\r\n");
	}
	else {
		// ���״δ���ʵ��
		ui.plainTextEdit->appendPlainText("First instance of Singleton:\r\n");
		ui.plainTextEdit->appendPlainText("--> Access application features now.\r\n");
	}

}

Singleton::~Singleton()
{
	// �ƺ���
	if (g_hSingleton != nullptr) {
		CloseHandle(g_hSingleton);
	}
	if (g_hNamespace != nullptr) {
		// ����Ǵ򿪵����������ռ� ��رվ��
		// ��������Ǳ�ʵ�������������ռ� ��˳�����������ռ�
		if (g_bNamespaceOpened) {
			ClosePrivateNamespace(g_hNamespace, 0);
		}
		else {
			ClosePrivateNamespace(g_hNamespace, PRIVATE_NAMESPACE_FLAG_DESTROY);
		}
	}
	if (g_hBoundary != nullptr) {
		DeleteBoundaryDescriptor(g_hBoundary);
	}
}

