#include "ProcessInfo.h"

ProcessInfo::ProcessInfo(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setMinimumSize(1500, 800);
	this->setWindowTitle("Process Info"); // 暂时先将标题设置为此
	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap); // 自动显示滚动条

	// 获取debug权限来查看服务信息
	CToolhelp::EnablePrivilege(SE_DEBUG_NAME, TRUE);
	// 获取访问sacl的权限
	CToolhelp::EnablePrivilege(SE_SECURITY_NAME, TRUE);


	// 做一些初始化和准备工作
	Init();

	// UAC选项
	connect(ui.actionSystem_processes, &QAction::triggered, [=]() {
		this->hide(); // 弹出UAC之前先隐藏程序
		// 获取我们进程的全路径
		TCHAR szApplication[MAX_PATH];
		DWORD cchLength = _countof(szApplication);
		QueryFullProcessImageName(GetCurrentProcess(), 0, szApplication, &cchLength);
		// 通过UAC请求重启
		DWORD dwStatus = StartElevatedProcess(szApplication, NULL);
		if (dwStatus == S_OK) {
			// 新进程重启成功 本进程结束
			this->close();
		}
		else
		{
			// UAC请求失败 再次显示窗口
			this->show();
		}
		});

	// 选择process模式并允许vmmap
	connect(ui.actionProcess_mode, &QAction::triggered, [=]() {
		ui.label->setText("Process mode");
		ui.actionVMmap_tool->setEnabled(true);
		PopulateProcessList();
		});
	// 选择module模式并禁止vmmap
	connect(ui.actionModule_mode, &QAction::triggered, [=]() {
		ui.label->setText("Module mode");
		ui.actionVMmap_tool->setEnabled(false);
		PopulateModuleList();
		});

	// 选择进程或模块显示信息
	void (QComboBox:: * activatedSignal)(int) = &QComboBox::activated;
	connect(ui.comboBox, activatedSignal, [=]() {
		// 可以通过查看vmmap是否被禁止来判断选择是进程模式还是模块模式
		if (ui.actionVMmap_tool->isEnabled()) {
			ShowProcessInfo((DWORD)ui.comboBox->currentData().toUInt());
		}
		else
		{
			const wchar_t* wstrTmp = reinterpret_cast<const wchar_t*>(ui.comboBox->currentData().toString().utf16());
			ShowModuleInfo((PCTSTR)wstrTmp);
		}
		});

	// 打开VMmap
	connect(ui.actionVMmap_tool, &QAction::triggered, [=]() {
		TCHAR szCmdLine[32];
		DWORD dwProcessId = (DWORD)ui.comboBox->currentData().toUInt();
		StringCchPrintf(szCmdLine, _countof(szCmdLine), TEXT("%d"),
			dwProcessId);

		DWORD dwStatus =
			StartElevatedProcess(TEXT("\"14-VMMap.exe\""), szCmdLine);
		if (dwStatus == ERROR_CANCELLED) {
			QMessageBox::critical(this, "error", "Failed to run 14-VMMap.exe: you refused access.");
		}
		});
}

ProcessInfo::~ProcessInfo() {
	// 对应构造函数 优雅地进行权限归位
	CToolhelp::EnablePrivilege(SE_SECURITY_NAME, FALSE);
	CToolhelp::EnablePrivilege(SE_DEBUG_NAME, FALSE);
}

// 将参数按格式化字符串拼接之后追加输出到textedit面板
// 介于QString和Unicode的转换频繁 还是需要把这个函数改造一下用起来
void ProcessInfo::AddText(PCTSTR pszFormat, ...) {
	// 这里采用的方式是每次都取出所有已存在内容
	// 然后将新内容拼接上去
	// 这样做的原因是如果采用每次直接添加内容至plainTextEdit
	// 会有很多格式上的诸多问题
	va_list argList;
	va_start(argList, pszFormat);

	TCHAR sz[20 * 1024];
	_tcscpy_s(sz, _countof(sz), reinterpret_cast<const wchar_t*>(ui.plainTextEdit->toPlainText().utf16()));
	_vstprintf_s(
		_tcschr(sz, TEXT('\0')), 
		_countof(sz) - _tcslen(sz),
		pszFormat, argList);
	ui.plainTextEdit->setPlainText(QString::fromWCharArray(sz));
	va_end(argList);
}

// 根据进程句柄获取指定进程的完整性级别
BOOL ProcessInfo::GetProcessIntegrityLevel(
	HANDLE hProcess,
	PDWORD pIntegrityLevel,
	PDWORD pPolicy,
	PDWORD pResourceIntegrityLevel,
	PDWORD pResourcePolicy) {
	// 请求以读权限打开指定进程的令牌内核对象
	HANDLE hToken = nullptr;
	if (!OpenProcessToken(hProcess, TOKEN_READ, &hToken)) {
		return(FALSE);
	}

	BOOL bReturn = FALSE; // 默认获取失败

	// 先计算存放完整性级别信息的结构的大小
	DWORD dwNeededSize = 0;
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, nullptr, 0, &dwNeededSize)) {
		PTOKEN_MANDATORY_LABEL pTokenInfo = nullptr;
		// 因为只是为了获取大小 所以没有传入有效buffer 会返回这样的错误信息
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			// 分配所需内存大小的堆块
			pTokenInfo = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, dwNeededSize);
			if (pTokenInfo != nullptr) {
				// 获取完整性级别信息
				if (GetTokenInformation(
					hToken,
					TokenIntegrityLevel,
					pTokenInfo,
					dwNeededSize,
					&dwNeededSize)) {
					// 这个函数获取一个SID的某个中间签发者（Sub Authority）的SID的指针
					*pIntegrityLevel =
						*GetSidSubAuthority(
							pTokenInfo->Label.Sid,
							(*GetSidSubAuthorityCount(pTokenInfo->Label.Sid) - 1));
					bReturn = TRUE; // 以获取完整性信息
				}

				// 回收堆空间
				LocalFree(pTokenInfo);
			}
		}
	}

	// 拿到已经获得的完整性信息中的强制性策略
	if (bReturn) {
		*pPolicy = TOKEN_MANDATORY_POLICY_OFF; // 默认没有针对令牌的强制性策略
		dwNeededSize = sizeof(DWORD);
		GetTokenInformation(
			hToken,
			TokenMandatoryPolicy,
			pPolicy,
			dwNeededSize,
			&dwNeededSize);
	}

	// 找到资源策略信息
	*pResourceIntegrityLevel = 0; // 默认为0表示没有显式设置
	*pResourcePolicy = 0;

	PACL                 pSACL = nullptr;
	PSECURITY_DESCRIPTOR pSD = nullptr;
	DWORD                dwResult = ERROR_SUCCESS;

	// Look for the no-read-up/no-write-up policy in the SACL
	// 在SACL中寻找禁读和禁写策略
	if (hToken != nullptr) {
		dwResult = GetSecurityInfo(
			hProcess,
			SE_KERNEL_OBJECT,
			LABEL_SECURITY_INFORMATION,
			nullptr,
			nullptr,
			nullptr,
			&pSACL,
			&pSD);
		if (dwResult == ERROR_SUCCESS) {
			if (pSACL != nullptr) {
				SYSTEM_MANDATORY_LABEL_ACE* pACE = nullptr;
				if ((pSACL->AceCount > 0) && (GetAce(pSACL, 0, (PVOID*)& pACE))) {
					if (pACE != nullptr) {
						SID* pSID = (SID*)(&pACE->SidStart);
						*pResourceIntegrityLevel = pSID->SubAuthority[0]; // 第一个ACE针对的SID的中间签发者
						*pResourcePolicy = pACE->Mask; // ACE的访问掩码
					}
				}
			}
		}
		// 清理分配的内存 主要是没用到的进程安全描述符
		if (pSD != nullptr) LocalFree(pSD);
	}
	// 关闭令牌句柄
	CloseHandle(hToken);

	return(bReturn);
}

// 根据pid和系统完整性级别信息获取指定进程的完整性级别
BOOL ProcessInfo::GetProcessIntegrityLevel(
	DWORD PID,
	PDWORD pIntegrityLevel,
	PDWORD pPolicy,
	PDWORD pResourceIntegrityLevel,
	PDWORD pResourcePolicy) {

	// 必须要有合法pid和完整性级别信息才可以执行函数
	if ((PID <= 0) || (pIntegrityLevel == nullptr))
		return(FALSE);

	// 打开进程句柄 顺便看看是否有权限获取信息
	HANDLE hProcess = OpenProcess(READ_CONTROL | PROCESS_QUERY_INFORMATION,
		FALSE,
		PID);

	if (hProcess == nullptr)
		return(FALSE);

	// 拿到句柄之后就可以交给上面那个函数了。。。。
	BOOL bReturn = GetProcessIntegrityLevel(
		hProcess,
		pIntegrityLevel,
		pPolicy,
		pResourceIntegrityLevel,
		pResourcePolicy);

	// 释放句柄
	CloseHandle(hProcess);

	return(bReturn);
}

// 在ComboBox显示进程
VOID ProcessInfo::PopulateProcessList() {
	// 这里先清除comboBox的内容
	ui.comboBox->clear();

	// 创建ctoolhelp对象来遍历进程快照
	CToolhelp thProcesses(TH32CS_SNAPPROCESS);
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = thProcesses.ProcessFirst(&pe);
	for (; fOk; fOk = thProcesses.ProcessNext(&pe)) {
		TCHAR sz[1024];

		// 把进程名称(不带路径)还有PID放进去
		// 先找exe名称 tcsrchr是查找字符串中某字符最后出现的位置
		PCTSTR pszExeFile = _tcsrchr(pe.szExeFile, TEXT('\\'));
		if (pszExeFile == NULL) {
			pszExeFile = pe.szExeFile;
		}
		else {
			pszExeFile++; // 跳过slash
		}

		// Append the code/resource integrity level and policy
		// 获取进程/资源的完整性级别和策略
		DWORD dwCodeIntegrityLevel = 0;
		DWORD dwCodePolicy = TOKEN_MANDATORY_POLICY_OFF;
		DWORD dwResourcePolicy = 0;
		DWORD dwResourceIntegrityLevel = 0;
		// 存放细节信息 默认为空字符串
		TCHAR szCodeDetails[256];
		szCodeDetails[0] = TEXT('\0');
		TCHAR szResourceDetails[256];
		szResourceDetails[0] = TEXT('\0');
		if (GetProcessIntegrityLevel(pe.th32ProcessID, &dwCodeIntegrityLevel,
			&dwCodePolicy, &dwResourceIntegrityLevel, &dwResourcePolicy)) {
			switch (dwCodeIntegrityLevel) {
			case SECURITY_MANDATORY_LOW_RID:
				_tcscpy_s(szCodeDetails, _countof(szCodeDetails),
					TEXT("- Low "));
				break;

			case SECURITY_MANDATORY_MEDIUM_RID:
				_tcscpy_s(szCodeDetails, _countof(szCodeDetails),
					TEXT("- Medium "));
				break;

			case SECURITY_MANDATORY_HIGH_RID:
				_tcscpy_s(szCodeDetails, _countof(szCodeDetails),
					TEXT("- High "));
				break;

			case SECURITY_MANDATORY_SYSTEM_RID:
				_tcscpy_s(szCodeDetails, _countof(szCodeDetails),
					TEXT("- System "));
				break;

			default:
				_tcscpy_s(szCodeDetails, _countof(szCodeDetails),
					TEXT("- ??? "));
			}

			if (dwCodePolicy == TOKEN_MANDATORY_POLICY_OFF) { // = 0
				_tcscat_s(szCodeDetails,
					_countof(szCodeDetails), TEXT(" + no policy"));
			}
			else {
				if ((dwCodePolicy & TOKEN_MANDATORY_POLICY_VALID_MASK) == 0) {
					_tcscat_s(szCodeDetails, _countof(szCodeDetails),
						TEXT(" + ???"));
				}
				else {
					if ((dwCodePolicy & TOKEN_MANDATORY_POLICY_NO_WRITE_UP)
						== TOKEN_MANDATORY_POLICY_NO_WRITE_UP) {
						_tcscat_s(szCodeDetails, _countof(szCodeDetails),
							TEXT(" + no write-up"));
					}

					if ((dwCodePolicy & TOKEN_MANDATORY_POLICY_NEW_PROCESS_MIN)
						== TOKEN_MANDATORY_POLICY_NEW_PROCESS_MIN) {
						_tcscat_s(szCodeDetails, _countof(szCodeDetails),
							TEXT(" + new process min"));
					}
				}
			}

			switch (dwResourceIntegrityLevel) {
			case SECURITY_MANDATORY_LOW_RID:
				_tcscpy_s(szResourceDetails,
					_countof(szResourceDetails), TEXT("Low"));
				break;

			case SECURITY_MANDATORY_MEDIUM_RID:
				_tcscpy_s(szResourceDetails,
					_countof(szResourceDetails), TEXT("Medium"));
				break;

			case SECURITY_MANDATORY_HIGH_RID:
				_tcscpy_s(szResourceDetails,
					_countof(szResourceDetails), TEXT("High"));
				break;

			case SECURITY_MANDATORY_SYSTEM_RID:
				_tcscpy_s(szResourceDetails,
					_countof(szResourceDetails), TEXT("System"));
				break;

			case 0:
				_tcscpy_s(szResourceDetails,
					_countof(szResourceDetails), TEXT("Not set"));
				break;

			default:
				_tcscpy_s(szResourceDetails,
					_countof(szResourceDetails), TEXT("???"));
			}


			if (dwResourcePolicy == 0) { // = 0
				_tcscat_s(szResourceDetails,
					_countof(szResourceDetails), TEXT(" + 0 policy"));
			}
			else {
				if ((dwResourcePolicy & TOKEN_MANDATORY_POLICY_VALID_MASK) == 0) {
					_tcscat_s(szResourceDetails,
						_countof(szResourceDetails), TEXT(" + ???"));
				}
				else {
					if ((dwResourcePolicy & SYSTEM_MANDATORY_LABEL_NO_WRITE_UP)
						== SYSTEM_MANDATORY_LABEL_NO_WRITE_UP) {
						_tcscat_s(szResourceDetails,
							_countof(szResourceDetails),
							TEXT(" + no write-up"));
					}

					if ((dwResourcePolicy & SYSTEM_MANDATORY_LABEL_NO_READ_UP)
						== SYSTEM_MANDATORY_LABEL_NO_READ_UP) {
						_tcscat_s(szResourceDetails,
							_countof(szResourceDetails),
							TEXT(" + no read-up"));
					}
					if ((dwResourcePolicy & SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP)
						== SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP) {
						_tcscat_s(szResourceDetails,
							_countof(szResourceDetails),
							TEXT(" + no execute-up"));
					}
				}
			}
		}

		StringCchPrintf(
			sz,
			_countof(sz),
			TEXT("%s     (0x%08X)  %s    [%s]"),
			pszExeFile,
			pe.th32ProcessID,
			szCodeDetails,
			szResourceDetails);
		// 添加至combobox表项，并将pid与之关联，这里编译32位程序就类型转换为可用的int了
		ui.comboBox->addItem(QString::fromWCharArray(sz), QVariant((unsigned int)pe.th32ProcessID));
	}

	// 这里可以模拟用户选择了第一个item
	ShowProcessInfo((DWORD)ui.comboBox->currentData().toUInt());
}

// 在comboBox显示模块
VOID ProcessInfo::PopulateModuleList() {
	// 清空comboBox
	ui.comboBox->clear();
	// 遍历进程快照 再以每一个PID 遍历其中的模块快照
	CToolhelp thProcesses(TH32CS_SNAPPROCESS);
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = thProcesses.ProcessFirst(&pe);
	for (; fOk; fOk = thProcesses.ProcessNext(&pe)) {

		CToolhelp thModules(TH32CS_SNAPMODULE, pe.th32ProcessID);
		MODULEENTRY32 me = { sizeof(me) };
		BOOL fOk = thModules.ModuleFirst(&me);
		for (; fOk; fOk = thModules.ModuleNext(&me)) {
			// 由于一个模块可能被多个进程载入，所以要先判断一下是否已经添加此模块
			int n = ui.comboBox->findData(QString::fromWCharArray(me.szExePath));
			if (n == -1) {
				// 将此模块名称添加入combobox 并关联至其路径全称
				ui.comboBox->addItem(
					QString::fromWCharArray(_tcsrchr(me.szExePath, TEXT('\\')) + 1),
					QVariant(QString::fromWCharArray(me.szExePath)));
			}
		}
	}
	// 这里可以模拟用户选择了一项item
	const wchar_t* wstrTmp = reinterpret_cast<const wchar_t*>(ui.comboBox->currentData().toString().utf16());
	ShowModuleInfo((PCTSTR)wstrTmp);
}

// 通过模块句柄和pid读取其PE头部信息获取模块默认加载基址
PVOID ProcessInfo::GetModulePreferredBaseAddr(DWORD dwProcessId, PVOID pvModuleRemote) {
	PVOID pvModulePreferredBaseAddr = nullptr;
	IMAGE_DOS_HEADER idh;
	IMAGE_NT_HEADERS inth;

	// 读取指定进程指定模块的dos头部
	Toolhelp32ReadProcessMemory(
		dwProcessId,
		pvModuleRemote,
		&idh,
		sizeof(idh),
		nullptr);

	// 验证一下是否是pe头
	if (idh.e_magic == IMAGE_DOS_SIGNATURE) {
		// 进一步读取nt头部
		Toolhelp32ReadProcessMemory(
			dwProcessId,
			(PBYTE)pvModuleRemote + idh.e_lfanew, // 根据dos头信息加上地址偏移
			&inth,
			sizeof(inth),
			nullptr);

		// 验证一下nt映像头
		if (inth.Signature == IMAGE_NT_SIGNATURE) {
			// 确定是合法nt头 读取其默认加载基址
			pvModulePreferredBaseAddr = (PVOID)inth.OptionalHeader.ImageBase;
		}
	}
	return(pvModulePreferredBaseAddr);
}

// 封装了显式导入的NtQueryInformationProcess
NTSTATUS ProcessInfo::_NtQueryInformationProcess(
	HANDLE hProcess,
	PROCESSINFOCLASS pic,
	PVOID pPI,
	ULONG cbSize,
	PULONG pLength
) {

	HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
	if (hNtDll == nullptr) {
		return(-1);
	}

	NTSTATUS lStatus = -1;  // 默认返回-1

	// 拿到NtQueryInformationProcess函数本尊
	// 注意：dll中的函数名都是char
	PFN_NTQUERYINFORMATIONPROCESS pfnNtQIP =
		(PFN_NTQUERYINFORMATIONPROCESS)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if (pfnNtQIP != nullptr) {
		lStatus = pfnNtQIP(hProcess, pic, pPI, cbSize, pLength);
	}
	// 释放动态库
	FreeLibrary(hNtDll);
	return(lStatus); // 返回NTSTATUS结构
}

// 通过进程句柄获取指定进程命令行信息
BOOL ProcessInfo::GetProcessCmdLine(HANDLE hProcess, LPTSTR szCmdLine, DWORD Size) {

	// 参数检查
	if ((hProcess == nullptr) || (szCmdLine == nullptr) || (Size == 0))
		return(FALSE);

	// 0. 首先要获得进程的PEB地址
	DWORD dwSize;
	SIZE_T size;
	PROCESS_BASIC_INFORMATION  pbi;
	// 尽管PEB结构在xp系统中始终是在进程虚拟地址空间中的0x7ffdf000位置
	// 但是PEB的地址在每一版本Windows中是有区别的
	// 这个才是拿到PEB的正确方法
	// 先通过NtQueryInformationProcess函数拿到PROCESS_BASIC_INFORMATION结构
	// NtQueryInformationProcess函数失败时会返回负数
	if (0 > _NtQueryInformationProcess(
		hProcess,
		ProcessBasicInformation,
		&pbi,
		sizeof(pbi),
		&dwSize)) {
		return FALSE;
	}

	// 1. 通过pbi提供的PEB地址找到进程环境快
	__PEB PEB;
	size = dwSize;
	if (!ReadProcessMemory(
		hProcess,
		pbi.PebBaseAddress,
		&PEB,
		sizeof(PEB),
		&size)) {
		return(FALSE);
	}

	// 2. 从PEB结构中获取指向包含进程命令行信息的结构的指针
	__INFOBLOCK Block;
	if (!ReadProcessMemory(
		hProcess,
		(LPVOID)PEB.InfoBlockAddress,
		&Block,
		sizeof(Block),
		&size)) {
		return(FALSE);
	}

	// 3. 获取命令行信息 注意 是用Unicode去接的
	wchar_t wszCmdLine[MAX_PATH + 1];
	if (!ReadProcessMemory(
		hProcess,
		(LPVOID)Block.wszCmdLineAddress,
		wszCmdLine,
		MAX_PATH * sizeof(wchar_t),
		&size)) {
		return(FALSE);
	}

	// 4. 跳过命令行信息中的文件路径这一段 它有可能是空字符串
	//    文件路径有可能是这两种形式"c:\...\app.exe" 或 c:\...\app.exe
	wchar_t* pPos = wszCmdLine; // 拿到第一个宽字符位置
	// 在第一个字符不是空字符的情况下
	if (*pPos != L'\0') {
		if (*pPos == L'"') {
			// 如果是双引号形式 就定位到下一个双引号
			pPos = wcschr(&pPos[1], L'"');
		}
		else {
			// Find the next SPACE character
			// 不然就是没有双引号的形式 直接定位到空格
			pPos = wcschr(&pPos[1], L' ');
		}

		// 跳过它 此时就已经在命令行参数部分了
		if (pPos != nullptr) // 这里其实是检验上面的wcschr是否执行成功
			pPos++;
	}

	// 将已经跳过文件路径的命令行信息复制回去
	if (pPos != nullptr && *pPos != L'\0') {
		// 如果存在的话
		// 由于PEB中信息是Unicode编码
		// 所以 如果程序的编码形式不同 就分了两种形式
#ifdef UNICODE
		_tcscpy_s(szCmdLine, Size, pPos);
#else
		MultiByteToWideChar(CP_ACP, 0, szCmdLine, Size,
			pPos, wcslen(pPos));
#endif
	}
	else
		szCmdLine[0] = TEXT('\0');
	// 所有都没问题 返回true
	return(TRUE);
}

// 通过pid获取指定进程命令行信息
BOOL ProcessInfo::GetProcessCmdLine(DWORD PID, LPTSTR szCmdLine, DWORD Size) {

	// 参数检查
	if ((PID <= 0) || (szCmdLine == nullptr))
		return(FALSE);

	// 打开指定进程对象 看是否有权限查看
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
	if (hProcess == nullptr)
		return(FALSE);
	// 老套路 执行上面版本的函数
	BOOL bReturn = GetProcessCmdLine(hProcess, szCmdLine, Size);

	// 释放句柄
	CloseHandle(hProcess);

	return(bReturn);
}

// 通过进程句柄获取进程所有者
BOOL ProcessInfo::GetProcessOwner(HANDLE hProcess, LPTSTR szOwner, size_t cchSize) {

	// 参数检查
	if ((szOwner == nullptr) || (cchSize == 0))
		return(FALSE);

	// 设置默认值
	szOwner[0] = TEXT('\0');

	// 拿到进程令牌句柄
	HANDLE hToken = nullptr;
	// 提升到一个比较高的权限（成为操作系统的一部分 以操作系统方式操作）
	CToolhelp::EnablePrivilege(SE_TCB_NAME, TRUE);
	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
		CToolhelp::EnablePrivilege(SE_TCB_NAME, FALSE);
		return(FALSE);
	}

	// 获取TOKEN_USER结构大小
	DWORD cbti = 0;
	GetTokenInformation(hToken, TokenUser, nullptr, 0, &cbti);

	// 这里我改为do-while形式 逻辑好理解一些
	do {
		// 由于刚才只是请求大小 所以返回这个错误值才是正常的
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			break;
		// 申请堆空间来存放结构体
		PTOKEN_USER ptiUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), 0, cbti);
		if (ptiUser == nullptr)
			break;

		// 拿到指定token的tokenuser信息
		if (GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti)) {
			SID_NAME_USE   snu;
			TCHAR          szUser[MAX_PATH];
			DWORD          chUser = MAX_PATH;
			PDWORD         pcchUser = &chUser;
			TCHAR          szDomain[MAX_PATH];
			DWORD          chDomain = MAX_PATH;
			PDWORD         pcchDomain = &chDomain;

			// 通过用户SID获取用户名和所在域名
			if (LookupAccountSid(
				nullptr,
				ptiUser->User.Sid,
				szUser,
				pcchUser,
				szDomain,
				pcchDomain,
				&snu)) {
				// 构造\\DomainName\UserName形式的字符串
				_tcscpy_s(szOwner, cchSize, TEXT("\\\\"));
				_tcscat_s(szOwner, cchSize, szDomain);
				_tcscat_s(szOwner, cchSize, TEXT("\\"));
				_tcscat_s(szOwner, cchSize, szUser);
			}
		}

		// 释放内存
		HeapFree(GetProcessHeap(), 0, ptiUser);

	} while (FALSE);

	// 释放句柄
	CloseHandle(hToken);

	// 重置权限设置
	CToolhelp::EnablePrivilege(SE_TCB_NAME, FALSE);

	return(TRUE);
}

// 通过进程pid获取进程所有者
BOOL ProcessInfo::GetProcessOwner(DWORD PID, LPTSTR szOwner, DWORD cchSize) {

	// 参数检查
	if ((PID <= 0) || (szOwner == nullptr))
		return(FALSE);

	// 打开进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
	if (hProcess == nullptr)
		return(FALSE);

	// 还是调用上面那个函数
	BOOL bReturn = GetProcessOwner(hProcess, szOwner, cchSize);

	// 释放句柄
	CloseHandle(hProcess);

	return(bReturn);
}

// 调整大小为KB单位
VOID ProcessInfo::FormatSizeInKB(
	DWORD dwSize,
	DWORD nCharacters,
	LPTSTR szSize,
	size_t cchSize) {

	TCHAR szFormattedSize[64];
	// 转换失败就没办法了。。。。
	if (StrFormatKBSize(dwSize, szFormattedSize, _countof(szFormattedSize)) == nullptr)
		StringCchPrintf(szFormattedSize, _countof(szFormattedSize), TEXT("%8u"), dwSize);

	// 将生成的字符串按要求向右标齐
	if (_tcslen(szFormattedSize) < nCharacters) {
		// 在前面添加适当个数的空格
		DWORD current = 0;
		for (current = 0;
			current < (nCharacters - _tcslen(szFormattedSize));
			current++) {
			szSize[current] = TEXT(' ');
		}
		szSize[current] = TEXT('\0'); // 要变为0结尾的字符串 才能调用下面的拼接函数
		_tcscat_s(szSize, cchSize - current, szFormattedSize);
	}
	else {
		_tcscpy_s(szSize, cchSize, szFormattedSize);
	}
}

// 根据pid在textedit显示指定进程信息
VOID ProcessInfo::ShowProcessInfo(DWORD dwProcessID) {

	ui.plainTextEdit->setPlainText(""); // 清空显示区

	CToolhelp th(TH32CS_SNAPALL, dwProcessID);

	// 显示进程的信息
	// 通过遍历进程快照 拿到PROCESSENTRY32结构
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = th.ProcessFirst(&pe);
	for (; fOk; fOk = th.ProcessNext(&pe)) {
		if (pe.th32ProcessID == dwProcessID) {
			// 显示命令行信息
			TCHAR szCmdLine[1024];
			if (GetProcessCmdLine(dwProcessID, szCmdLine, _countof(szCmdLine))) {
				AddText(TEXT("Command line: %s %s\r\n"), pe.szExeFile, szCmdLine);
			}
			else {
				AddText(TEXT("Filename: %s\r\n"), pe.szExeFile);
			}
			// 显示细节信息
			AddText(
				TEXT("   PID=%08X, ParentPID=%08X, PriorityClass=%d, Threads=%d, Heaps=%d\r\n"),
				pe.th32ProcessID,
				pe.th32ParentProcessID,
				pe.pcPriClassBase,
				pe.cntThreads,
				th.HowManyHeaps());
			// 显示进程所有者
			TCHAR szOwner[MAX_PATH + 1];
			if (GetProcessOwner(dwProcessID, szOwner, MAX_PATH)) {
				AddText(TEXT("Owner: %s\r\n"), szOwner);
			}

			break;   // 找到指定进程并显示完信息后就跳走
		}
	}


	// 显示进程中的模块信息
	// 把标题写好
	AddText(
		TEXT("\r\nModules Information:\r\n  Usage  %-*s(%-*s)  %10s  Module\r\n"),
		s_cchAddress, TEXT("BaseAddr"), // 这俩都是按s_cchAddress靠左对齐
		s_cchAddress, TEXT("ImagAddr"),
		TEXT("Size"));
	// 遍历模块快照 挨个输出模块信息
	// 由于在CToolhelp中对函数进行了封装
	// 初始化对象之后 这里可以直接遍历该进程的模块
	MODULEENTRY32 me = { sizeof(me) };
	fOk = th.ModuleFirst(&me);
	for (; fOk; fOk = th.ModuleNext(&me)) {
		// 打印该模块的使用计数
		if (me.ProccntUsage == 65535) {
			// 这表示模块是隐式载入的 而且不能被卸载
			AddText(TEXT("  Fixed"));
		}
		else {
			AddText(TEXT("  %5d"), me.ProccntUsage);
		}
		// 将模块大小单位化为KB
		TCHAR szFormattedSize[64];
		if (StrFormatKBSize(me.modBaseSize, szFormattedSize, _countof(szFormattedSize)) == nullptr)
		{
			StringCchPrintf(szFormattedSize, _countof(szFormattedSize), TEXT("%10u"), me.modBaseSize);
		}
		// 获取模块默认加载基址 并打印获取到的模块信息
		PVOID pvPreferredBaseAddr = GetModulePreferredBaseAddr(pe.th32ProcessID, me.modBaseAddr);
		if (me.modBaseAddr == pvPreferredBaseAddr) {
			AddText(
				TEXT("  %p %*s   %10s  %s\r\n"),
				me.modBaseAddr,
				s_cchAddress, TEXT(""), // 这表示按s_cchAddress宽度向右对齐
				szFormattedSize,
				me.szExePath);
		}
		else {
			// 模块没有加载到默认基址 就把默认加载地址和实际加载地址都写上
			AddText(
				TEXT("  %p(%p)  %10s  %s\r\n"),
				me.modBaseAddr,
				pvPreferredBaseAddr,
				szFormattedSize,
				me.szExePath);
		}
	}

	// 显示该进程中的线程
	AddText(TEXT("\r\nThread Information:\r\n      TID     Priority\r\n"));
	THREADENTRY32 te = { sizeof(te) };
	fOk = th.ThreadFirst(&te);
	for (; fOk; fOk = th.ThreadNext(&te)) {
		if (te.th32OwnerProcessID == dwProcessID) {
			// 计算其相对优先级
			int nPriority = te.tpBasePri + te.tpDeltaPri;
			if ((te.tpBasePri < 16) && (nPriority > 15)) nPriority = 15;
			if ((te.tpBasePri > 15) && (nPriority > 31)) nPriority = 31;
			if ((te.tpBasePri < 16) && (nPriority < 1)) nPriority = 1;
			if ((te.tpBasePri > 15) && (nPriority < 16)) nPriority = 16;
			AddText(TEXT("   %08X       %2d\r\n"), te.th32ThreadID, nPriority);
		}
	}
	// 将光标移到开头
	ui.plainTextEdit->moveCursor(QTextCursor::Start);
}

// 根据模块路径 显示指定模块信息
VOID ProcessInfo::ShowModuleInfo(PCTSTR pszModulePath) {

	ui.plainTextEdit->setPlainText(""); // 清空textedit
	// 把标题写好
	AddText(TEXT("Pathname: %s\r\n\r\n"), pszModulePath);
	AddText(TEXT("Process Information:\r\n"));
	AddText(
		TEXT("     PID    %-*s  Process\r\n"),
		s_cchAddress, TEXT("BaseAddr")); // 按s_cchAddres向左对齐

// 通过快照技术 双重循环遍历所有进程中的所有模块 找到指定的那一个
	CToolhelp thProcesses(TH32CS_SNAPPROCESS);
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = thProcesses.ProcessFirst(&pe);
	// 进程循环
	for (; fOk; fOk = thProcesses.ProcessNext(&pe)) {
		CToolhelp thModules(TH32CS_SNAPMODULE, pe.th32ProcessID);
		MODULEENTRY32 me = { sizeof(me) };
		BOOL fOk = thModules.ModuleFirst(&me);
		// 对应进程中的快照循环
		for (; fOk; fOk = thModules.ModuleNext(&me)) {
			// 比较
			if (_tcscmp(me.szExePath, pszModulePath) == 0) {
				AddText(TEXT("  %08X  %p  %s\r\n"),
					pe.th32ProcessID,
					me.modBaseAddr,
					pe.szExeFile);
			}
		}
	}
	// 将光标移到开头
	ui.plainTextEdit->moveCursor(QTextCursor::Start);
}

// 判断当前用户是否是管理员 并获取当前进程提升信息
BOOL ProcessInfo::GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin) {

	HANDLE hToken = nullptr;
	DWORD dwSize;

	// 先拿到自己的令牌
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return(FALSE);

	BOOL bResult = FALSE;

	// 获取提升类型信息
	if (GetTokenInformation(
		hToken,
		TokenElevationType,
		pElevationType, // 为了接收这个枚举值
		sizeof(TOKEN_ELEVATION_TYPE),
		&dwSize)) {
		// 创建一个关联管理员组的通用SID
		byte adminSID[SECURITY_MAX_SID_SIZE];
		dwSize = sizeof(adminSID);
		CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, &adminSID, &dwSize);

		// 判断当前提升类型是否被筛选过
		if (*pElevationType == TokenElevationTypeLimited) {
			// Get handle to linked token (will have one if we are lua)
			// 当前是被筛选出来的 那么就拿到被其他相关联的未被筛选的令牌信息
			HANDLE hUnfilteredToken = nullptr;
			GetTokenInformation(
				hToken,
				TokenLinkedToken,
				(VOID*)& hUnfilteredToken,
				sizeof(HANDLE),
				&dwSize);

			// 检查其中是否有一个管理员账号SID
			if (CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin)) {
				bResult = TRUE;
			}

			// 关闭句柄
			CloseHandle(hUnfilteredToken);
		}
		else {
			// 如果当前不是被筛选出来的令牌 直接判断就可以知道当前用户是不是管路员了
			*pIsAdmin = IsUserAnAdmin();
			bResult = TRUE;
		}
	}

	// 关闭句柄
	CloseHandle(hToken);

	return(bResult);
}

// 一些初始化工作
VOID ProcessInfo::Init() {
	// 如果下面判断当前是以管理员身份运行程序 就设置为TRUE
	BOOL bCanReadSystemProcesses = FALSE;

	// 看看我们是否是在筛选令牌下运行
	if (GetProcessElevation(&s_elevationType, &s_bIsAdmin)) {
		// prefix title with elevation
		// 用来保存我们拿来替换的窗口标题前缀
		TCHAR szTitle[64];

		switch (s_elevationType) {
			// 默认用户或者是UAC请求被拒绝
		case TokenElevationTypeDefault:
			if (IsUserAnAdmin()) {
				_tcscpy_s(szTitle, _countof(szTitle), TEXT("Default Administrator: "));
				bCanReadSystemProcesses = true;
			}
			else {
				_tcscpy_s(szTitle, _countof(szTitle),
					TEXT("Default: "));
			}
			break;

			// 进程提权成功
		case TokenElevationTypeFull:
			if (IsUserAnAdmin()) {
				_tcscpy_s(szTitle, _countof(szTitle),
					TEXT("Elevated Administrator: "));
				bCanReadSystemProcesses = true;
			}
			else {
				_tcscpy_s(szTitle, _countof(szTitle),
					TEXT("Elevated: "));
			}
			break;

			// 进程在筛选权限下运行
		case TokenElevationTypeLimited:
			if (s_bIsAdmin) {
				_tcscpy_s(szTitle, _countof(szTitle),
					TEXT("Filtered Administrator: "));
			}
			else {
				_tcscpy_s(szTitle, _countof(szTitle),
					TEXT("Filtered: "));
			}
			break;
		}

		// 将窗口标题更新为对应用户模式
		this->setWindowTitle(QString::fromWCharArray(szTitle) + this->windowTitle());

		// 如果已经是管理员模式，那就不需要这个UAC请求按钮了
		if (bCanReadSystemProcesses) {
			ui.actionSystem_processes->setEnabled(false);
		}
	}
	// 显示进程模式
	ui.label->setText("Process mode");
	// 主动先加载进程comboBox
	PopulateProcessList();
}

// 征求用户意见 以管理员权限启动指定程序
DWORD ProcessInfo::StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine) {

	// 初始化结构
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = szExecutable;
	sei.lpParameters = szCmdLine;
	sei.nShow = SW_SHOWNORMAL; // 这一步别忘了 不然没有窗口显示

	ShellExecuteEx(&sei);
	return(GetLastError());
}

