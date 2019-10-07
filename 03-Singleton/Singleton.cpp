#include "Singleton.h"

Singleton::Singleton(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(1000, 450);
	this->setWindowTitle("Singleton");
	ui.plainTextEdit->setReadOnly(true);
	ui.pushButton->setFixedWidth(120);

	// 点击Exit按钮关闭程序
	connect(ui.pushButton, &QPushButton::clicked, this, &QWidget::close);
	// 主逻辑
	CheckInstance();
}

void Singleton::CheckInstance() {
	// 创建边界描述符
	g_hBoundary = CreateBoundaryDescriptor(g_szBoundary, 0);

	// 创建一个关联本地管理员组的通用SID
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

	// 将创建好的本地管理员用户组SID与边界描述符关联
	// 这样一来 只有当程序在管理员用户下运行的时候
	// 才可以访问之后与此边界描述符关联的同一个命名空间的内核对象
	if (!AddSIDToBoundaryDescriptor(&g_hBoundary, pLocalAdminSID)) {
		ui.plainTextEdit->appendPlainText(QString("AddSIDToBoundaryDescriptor failed: %1\r\n").arg(GetLastError()));
		return;
	}

	// 创建只有本地管理员才能使用的命名空间
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;
	// 将一个按安全描述符格式的字符串转换成一个有效的安全描述符结构
	// 这个安全描述符是给命名空间的sa用的 不是刚刚边界描述符的
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
		TEXT("D:(A;;GA;;;BA)"), // 表示所创建的安全描述符所属对象只属于本地管理员
		SDDL_REVISION_1,
		&sa.lpSecurityDescriptor,
		nullptr)) {
		ui.plainTextEdit->appendPlainText(QString("Security Descriptor creation failed: %1\r\n").arg(GetLastError()));
		return;
	}
	// 正式创建带有边界的命名空间
	g_hNamespace = CreatePrivateNamespace(&sa, g_hBoundary, g_szNamespace);
	// 释放给命名空间存放安全描述符生成的堆空间
	LocalFree(sa.lpSecurityDescriptor);

	// 检查命名空间的创建情况 并做相应处理
	DWORD dwLastError = GetLastError();
	// 把这里万恶的if-else逻辑改的好看了一点
	// 不然作者写的逻辑难以下咽....
	if (g_hNamespace == nullptr) {
		if (dwLastError == ERROR_ACCESS_DENIED) {
			// 这种情况是权限不够 程序必须由管理员来运行
			ui.plainTextEdit->appendPlainText("Access denied when creating the namespace.\r\n");
			ui.plainTextEdit->appendPlainText("	You must be running as Administrator.\r\n\r\n");
			return;
		}
		else if (dwLastError == ERROR_ALREADY_EXISTS) {
			// 已经有其他实例创建了这个命名空间
			// 那么我们就打开它
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


	// 在上文的命名空间中创建一个命名互斥量对象
	// 构造命名对象的名称（包含所指命名空间信息）
	TCHAR szMutexName[64];
	StringCchPrintf(szMutexName, _countof(szMutexName), TEXT("%s\\%s"), g_szNamespace, TEXT("Singleton"));
	// 创建互斥量对象
	g_hSingleton = CreateMutex(nullptr, FALSE, szMutexName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// 已经存在一个实例
		ui.plainTextEdit->appendPlainText("Another instance of Singleton is running:\r\n");
		ui.plainTextEdit->appendPlainText("--> Impossible to access application features.\r\n");
	}
	else {
		// 是首次创建实例
		ui.plainTextEdit->appendPlainText("First instance of Singleton:\r\n");
		ui.plainTextEdit->appendPlainText("--> Access application features now.\r\n");
	}

}

Singleton::~Singleton()
{
	// 善后工作
	if (g_hSingleton != nullptr) {
		CloseHandle(g_hSingleton);
	}
	if (g_hNamespace != nullptr) {
		// 如果是打开的已有命名空间 则关闭句柄
		// 否则如果是本实例创建的命名空间 就顺带销毁命名空间
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

