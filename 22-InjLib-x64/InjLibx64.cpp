#include "InjLibx64.h"

InjLibx64::InjLibx64(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//-----------------------------�������---------------------------------
	this->setWindowTitle("Inject Library Tester");
	this->setFixedSize(800, 200);

	ui.buttonChoose->setText("Choose DLL");

	ui.lineEditPath->setReadOnly(true);

	ui.labelPid->setText("Process Id (decimal):");

	ui.lineEditPid->setValidator(new QIntValidator(this));

	ui.lineEditPid->setFixedWidth(100);

	ui.lineEditPid->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

	ui.buttonInj->setText("Inject");

	ui.buttonElevate->setText("Elevate!");

	//-----------------------------����������---------------------------------

	ui.buttonInj->setEnabled(false);

	// ��ť�߼�
	connect(ui.lineEditPid, &QLineEdit::textChanged, [=](const QString& text) {
		// ֻ���û��Ѿ����ú���dll·����pid֮��ſ�������inject
		if (!text.isEmpty() && pFilePath != NULL) {
			ui.buttonInj->setEnabled(true);
		}
		else
		{
			ui.buttonInj->setEnabled(false);
		}
		});

	// ѡ��dll
	connect(ui.buttonChoose, &QPushButton::clicked, [=]() {

		QString qsFilePath = QFileDialog::getOpenFileName(this, "Choose a DLL", "./", "(*.dll)");
		if (qsFilePath.isEmpty())
			return;

		ui.lineEditPath->setText(qsFilePath);
		pFilePath = reinterpret_cast<const wchar_t*>(qsFilePath.utf16());
		});

	// inject���߼�
	connect(ui.buttonInj, &QPushButton::clicked, [=]() {

		DWORD dwProcessId = ui.lineEditPid->text().toUInt();
		if (dwProcessId == 0) {
			// �������pid��������0 ��������Ϊ�˲���
			// �ͽ�pidΪ0���������Ϊע������
			dwProcessId = GetCurrentProcessId();
		}

		if (InjectLibW(dwProcessId, pFilePath)) {
			QMessageBox::information(this, "Congratulation!", "DLL Injection/Ejection successful.");

		}
		else {
			QMessageBox::warning(this, "Sorry about that", "DLL Injection/Ejection failed.");
		}
		});
}

// Զ�߳�ע���߼�
BOOL WINAPI InjLibx64::InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {

	BOOL bOk = FALSE;
	HANDLE hProcess = NULL, hThread = NULL;
	PWSTR pszLibFileRemote = NULL;

	__try {
		// ��ȡĿ����̾��
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |   // ʼ����Ҫ��Ȩ��
			PROCESS_CREATE_THREAD |   // ����Զ�߳�
			PROCESS_VM_OPERATION |   // VirtualAllocEx/VirtualFreeEx
			PROCESS_VM_WRITE,             // WriteProcessMemory
			FALSE, dwProcessId);
		if (hProcess == NULL) __leave;

		// ������Ҫд��dll·���ַ���С
		int cch = 1 + lstrlenW(pszLibFile); // ��һ�����β
		int cb = cch * sizeof(wchar_t);

		// ��Ŀ���������ռ�
		pszLibFileRemote = (PWSTR)
			VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL) __leave;

		// д��dll·��
		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibFile, cb, NULL)) __leave;

		// ��ȡ�����̵ģ����н��̶�һ���ģ�LoadLibraryW �� Kernel32.dll�еĵ�ַ
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == NULL) __leave;

		// ��Ŀ������д����̵߳���LoadLibraryW����
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pfnThreadRtn, pszLibFileRemote, 0, NULL);
		if (hThread == NULL) __leave;

		// �ȴ������ɹ� Ҳ���ǵ���LoadLibraryW���
		WaitForSingleObject(hThread, INFINITE);

		bOk = TRUE; // ����
	}
	__finally { // ����SEH������������

		// �ͷ�Ŀ����̵�ַ�ռ�
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if (hThread != NULL)
			CloseHandle(hThread);

		if (hProcess != NULL)
			CloseHandle(hProcess);
	}

	return(bOk);
}

