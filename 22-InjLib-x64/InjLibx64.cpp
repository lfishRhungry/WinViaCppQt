#include "InjLibx64.h"

InjLibx64::InjLibx64(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//-----------------------------界面设计---------------------------------
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

	//-----------------------------界面设计完毕---------------------------------

	ui.buttonInj->setEnabled(false);

	// 按钮逻辑
	connect(ui.lineEditPid, &QLineEdit::textChanged, [=](const QString& text) {
		// 只有用户已经设置好了dll路径和pid之后才可以启用inject
		if (!text.isEmpty() && pFilePath != NULL) {
			ui.buttonInj->setEnabled(true);
		}
		else
		{
			ui.buttonInj->setEnabled(false);
		}
		});

	// 选择dll
	connect(ui.buttonChoose, &QPushButton::clicked, [=]() {

		QString qsFilePath = QFileDialog::getOpenFileName(this, "Choose a DLL", "./", "(*.dll)");
		if (qsFilePath.isEmpty())
			return;

		ui.lineEditPath->setText(qsFilePath);
		pFilePath = reinterpret_cast<const wchar_t*>(qsFilePath.utf16());
		});

	// inject主逻辑
	connect(ui.buttonInj, &QPushButton::clicked, [=]() {

		DWORD dwProcessId = ui.lineEditPid->text().toUInt();
		if (dwProcessId == 0) {
			// 本来如果pid并不能是0 但是这里为了测试
			// 就将pid为0的情况设置为注入自身
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

// 远线程注入逻辑
BOOL WINAPI InjLibx64::InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {

	BOOL bOk = FALSE;
	HANDLE hProcess = NULL, hThread = NULL;
	PWSTR pszLibFileRemote = NULL;

	__try {
		// 获取目标进程句柄
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |   // 始终需要的权限
			PROCESS_CREATE_THREAD |   // 创建远线程
			PROCESS_VM_OPERATION |   // VirtualAllocEx/VirtualFreeEx
			PROCESS_VM_WRITE,             // WriteProcessMemory
			FALSE, dwProcessId);
		if (hProcess == NULL) __leave;

		// 计算需要写入dll路径字符大小
		int cch = 1 + lstrlenW(pszLibFile); // 加一个零结尾
		int cb = cch * sizeof(wchar_t);

		// 在目标进程申请空间
		pszLibFileRemote = (PWSTR)
			VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL) __leave;

		// 写入dll路径
		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibFile, cb, NULL)) __leave;

		// 获取本进程的（所有进程都一样的）LoadLibraryW 在 Kernel32.dll中的地址
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == NULL) __leave;

		// 在目标进程中创建线程调用LoadLibraryW函数
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pfnThreadRtn, pszLibFileRemote, 0, NULL);
		if (hThread == NULL) __leave;

		// 等待创建成功 也就是调用LoadLibraryW完成
		WaitForSingleObject(hThread, INFINITE);

		bOk = TRUE; // 完美
	}
	__finally { // 利用SEH性质来做清理

		// 释放目标进程地址空间
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if (hThread != NULL)
			CloseHandle(hThread);

		if (hProcess != NULL)
			CloseHandle(hProcess);
	}

	return(bOk);
}

