#include "ProcessInfo.h"

ProcessInfo::ProcessInfo(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setMinimumSize(1500, 800);
	this->setWindowTitle("Process Info"); // ��ʱ�Ƚ���������Ϊ��
	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap); // �Զ���ʾ������

	// ��ȡdebugȨ�����鿴������Ϣ
	CToolhelp::EnablePrivilege(SE_DEBUG_NAME, TRUE);
	// ��ȡ����sacl��Ȩ��
	CToolhelp::EnablePrivilege(SE_SECURITY_NAME, TRUE);


	// ��һЩ��ʼ����׼������
	Init();

	// UACѡ��
	connect(ui.actionSystem_processes, &QAction::triggered, [=]() {
		this->hide(); // ����UAC֮ǰ�����س���
		// ��ȡ���ǽ��̵�ȫ·��
		TCHAR szApplication[MAX_PATH];
		DWORD cchLength = _countof(szApplication);
		QueryFullProcessImageName(GetCurrentProcess(), 0, szApplication, &cchLength);
		// ͨ��UAC��������
		DWORD dwStatus = StartElevatedProcess(szApplication, NULL);
		if (dwStatus == S_OK) {
			// �½��������ɹ� �����̽���
			this->close();
		}
		else
		{
			// UAC����ʧ�� �ٴ���ʾ����
			this->show();
		}
		});

	// ѡ��processģʽ������vmmap
	connect(ui.actionProcess_mode, &QAction::triggered, [=]() {
		ui.label->setText("Process mode");
		ui.actionVMmap_tool->setEnabled(true);
		PopulateProcessList();
		});
	// ѡ��moduleģʽ����ֹvmmap
	connect(ui.actionModule_mode, &QAction::triggered, [=]() {
		ui.label->setText("Module mode");
		ui.actionVMmap_tool->setEnabled(false);
		PopulateModuleList();
		});

	// ѡ����̻�ģ����ʾ��Ϣ
	void (QComboBox:: * activatedSignal)(int) = &QComboBox::activated;
	connect(ui.comboBox, activatedSignal, [=]() {
		// ����ͨ���鿴vmmap�Ƿ񱻽�ֹ���ж�ѡ���ǽ���ģʽ����ģ��ģʽ
		if (ui.actionVMmap_tool->isEnabled()) {
			ShowProcessInfo((DWORD)ui.comboBox->currentData().toUInt());
		}
		else
		{
			const wchar_t* wstrTmp = reinterpret_cast<const wchar_t*>(ui.comboBox->currentData().toString().utf16());
			ShowModuleInfo((PCTSTR)wstrTmp);
		}
		});

	// ��VMmap
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
	// ��Ӧ���캯�� ���ŵؽ���Ȩ�޹�λ
	CToolhelp::EnablePrivilege(SE_SECURITY_NAME, FALSE);
	CToolhelp::EnablePrivilege(SE_DEBUG_NAME, FALSE);
}

// ����������ʽ���ַ���ƴ��֮��׷�������textedit���
// ����QString��Unicode��ת��Ƶ�� ������Ҫ�������������һ��������
void ProcessInfo::AddText(PCTSTR pszFormat, ...) {
	// ������õķ�ʽ��ÿ�ζ�ȡ�������Ѵ�������
	// Ȼ��������ƴ����ȥ
	// ��������ԭ�����������ÿ��ֱ�����������plainTextEdit
	// ���кܶ��ʽ�ϵ��������
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

// ���ݽ��̾����ȡָ�����̵������Լ���
BOOL ProcessInfo::GetProcessIntegrityLevel(
	HANDLE hProcess,
	PDWORD pIntegrityLevel,
	PDWORD pPolicy,
	PDWORD pResourceIntegrityLevel,
	PDWORD pResourcePolicy) {
	// �����Զ�Ȩ�޴�ָ�����̵������ں˶���
	HANDLE hToken = nullptr;
	if (!OpenProcessToken(hProcess, TOKEN_READ, &hToken)) {
		return(FALSE);
	}

	BOOL bReturn = FALSE; // Ĭ�ϻ�ȡʧ��

	// �ȼ����������Լ�����Ϣ�Ľṹ�Ĵ�С
	DWORD dwNeededSize = 0;
	if (!GetTokenInformation(hToken, TokenIntegrityLevel, nullptr, 0, &dwNeededSize)) {
		PTOKEN_MANDATORY_LABEL pTokenInfo = nullptr;
		// ��Ϊֻ��Ϊ�˻�ȡ��С ����û�д�����Чbuffer �᷵�������Ĵ�����Ϣ
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			// ���������ڴ��С�Ķѿ�
			pTokenInfo = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, dwNeededSize);
			if (pTokenInfo != nullptr) {
				// ��ȡ�����Լ�����Ϣ
				if (GetTokenInformation(
					hToken,
					TokenIntegrityLevel,
					pTokenInfo,
					dwNeededSize,
					&dwNeededSize)) {
					// ���������ȡһ��SID��ĳ���м�ǩ���ߣ�Sub Authority����SID��ָ��
					*pIntegrityLevel =
						*GetSidSubAuthority(
							pTokenInfo->Label.Sid,
							(*GetSidSubAuthorityCount(pTokenInfo->Label.Sid) - 1));
					bReturn = TRUE; // �Ի�ȡ��������Ϣ
				}

				// ���նѿռ�
				LocalFree(pTokenInfo);
			}
		}
	}

	// �õ��Ѿ���õ���������Ϣ�е�ǿ���Բ���
	if (bReturn) {
		*pPolicy = TOKEN_MANDATORY_POLICY_OFF; // Ĭ��û��������Ƶ�ǿ���Բ���
		dwNeededSize = sizeof(DWORD);
		GetTokenInformation(
			hToken,
			TokenMandatoryPolicy,
			pPolicy,
			dwNeededSize,
			&dwNeededSize);
	}

	// �ҵ���Դ������Ϣ
	*pResourceIntegrityLevel = 0; // Ĭ��Ϊ0��ʾû����ʽ����
	*pResourcePolicy = 0;

	PACL                 pSACL = nullptr;
	PSECURITY_DESCRIPTOR pSD = nullptr;
	DWORD                dwResult = ERROR_SUCCESS;

	// Look for the no-read-up/no-write-up policy in the SACL
	// ��SACL��Ѱ�ҽ����ͽ�д����
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
						*pResourceIntegrityLevel = pSID->SubAuthority[0]; // ��һ��ACE��Ե�SID���м�ǩ����
						*pResourcePolicy = pACE->Mask; // ACE�ķ�������
					}
				}
			}
		}
		// ���������ڴ� ��Ҫ��û�õ��Ľ��̰�ȫ������
		if (pSD != nullptr) LocalFree(pSD);
	}
	// �ر����ƾ��
	CloseHandle(hToken);

	return(bReturn);
}

// ����pid��ϵͳ�����Լ�����Ϣ��ȡָ�����̵������Լ���
BOOL ProcessInfo::GetProcessIntegrityLevel(
	DWORD PID,
	PDWORD pIntegrityLevel,
	PDWORD pPolicy,
	PDWORD pResourceIntegrityLevel,
	PDWORD pResourcePolicy) {

	// ����Ҫ�кϷ�pid�������Լ�����Ϣ�ſ���ִ�к���
	if ((PID <= 0) || (pIntegrityLevel == nullptr))
		return(FALSE);

	// �򿪽��̾�� ˳�㿴���Ƿ���Ȩ�޻�ȡ��Ϣ
	HANDLE hProcess = OpenProcess(READ_CONTROL | PROCESS_QUERY_INFORMATION,
		FALSE,
		PID);

	if (hProcess == nullptr)
		return(FALSE);

	// �õ����֮��Ϳ��Խ��������Ǹ������ˡ�������
	BOOL bReturn = GetProcessIntegrityLevel(
		hProcess,
		pIntegrityLevel,
		pPolicy,
		pResourceIntegrityLevel,
		pResourcePolicy);

	// �ͷž��
	CloseHandle(hProcess);

	return(bReturn);
}

// ��ComboBox��ʾ����
VOID ProcessInfo::PopulateProcessList() {
	// ���������comboBox������
	ui.comboBox->clear();

	// ����ctoolhelp�������������̿���
	CToolhelp thProcesses(TH32CS_SNAPPROCESS);
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = thProcesses.ProcessFirst(&pe);
	for (; fOk; fOk = thProcesses.ProcessNext(&pe)) {
		TCHAR sz[1024];

		// �ѽ�������(����·��)����PID�Ž�ȥ
		// ����exe���� tcsrchr�ǲ����ַ�����ĳ�ַ������ֵ�λ��
		PCTSTR pszExeFile = _tcsrchr(pe.szExeFile, TEXT('\\'));
		if (pszExeFile == NULL) {
			pszExeFile = pe.szExeFile;
		}
		else {
			pszExeFile++; // ����slash
		}

		// Append the code/resource integrity level and policy
		// ��ȡ����/��Դ�������Լ���Ͳ���
		DWORD dwCodeIntegrityLevel = 0;
		DWORD dwCodePolicy = TOKEN_MANDATORY_POLICY_OFF;
		DWORD dwResourcePolicy = 0;
		DWORD dwResourceIntegrityLevel = 0;
		// ���ϸ����Ϣ Ĭ��Ϊ���ַ���
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
		// �����combobox�������pid��֮�������������32λ���������ת��Ϊ���õ�int��
		ui.comboBox->addItem(QString::fromWCharArray(sz), QVariant((unsigned int)pe.th32ProcessID));
	}

	// �������ģ���û�ѡ���˵�һ��item
	ShowProcessInfo((DWORD)ui.comboBox->currentData().toUInt());
}

// ��comboBox��ʾģ��
VOID ProcessInfo::PopulateModuleList() {
	// ���comboBox
	ui.comboBox->clear();
	// �������̿��� ����ÿһ��PID �������е�ģ�����
	CToolhelp thProcesses(TH32CS_SNAPPROCESS);
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = thProcesses.ProcessFirst(&pe);
	for (; fOk; fOk = thProcesses.ProcessNext(&pe)) {

		CToolhelp thModules(TH32CS_SNAPMODULE, pe.th32ProcessID);
		MODULEENTRY32 me = { sizeof(me) };
		BOOL fOk = thModules.ModuleFirst(&me);
		for (; fOk; fOk = thModules.ModuleNext(&me)) {
			// ����һ��ģ����ܱ�����������룬����Ҫ���ж�һ���Ƿ��Ѿ���Ӵ�ģ��
			int n = ui.comboBox->findData(QString::fromWCharArray(me.szExePath));
			if (n == -1) {
				// ����ģ�����������combobox ����������·��ȫ��
				ui.comboBox->addItem(
					QString::fromWCharArray(_tcsrchr(me.szExePath, TEXT('\\')) + 1),
					QVariant(QString::fromWCharArray(me.szExePath)));
			}
		}
	}
	// �������ģ���û�ѡ����һ��item
	const wchar_t* wstrTmp = reinterpret_cast<const wchar_t*>(ui.comboBox->currentData().toString().utf16());
	ShowModuleInfo((PCTSTR)wstrTmp);
}

// ͨ��ģ������pid��ȡ��PEͷ����Ϣ��ȡģ��Ĭ�ϼ��ػ�ַ
PVOID ProcessInfo::GetModulePreferredBaseAddr(DWORD dwProcessId, PVOID pvModuleRemote) {
	PVOID pvModulePreferredBaseAddr = nullptr;
	IMAGE_DOS_HEADER idh;
	IMAGE_NT_HEADERS inth;

	// ��ȡָ������ָ��ģ���dosͷ��
	Toolhelp32ReadProcessMemory(
		dwProcessId,
		pvModuleRemote,
		&idh,
		sizeof(idh),
		nullptr);

	// ��֤һ���Ƿ���peͷ
	if (idh.e_magic == IMAGE_DOS_SIGNATURE) {
		// ��һ����ȡntͷ��
		Toolhelp32ReadProcessMemory(
			dwProcessId,
			(PBYTE)pvModuleRemote + idh.e_lfanew, // ����dosͷ��Ϣ���ϵ�ַƫ��
			&inth,
			sizeof(inth),
			nullptr);

		// ��֤һ��ntӳ��ͷ
		if (inth.Signature == IMAGE_NT_SIGNATURE) {
			// ȷ���ǺϷ�ntͷ ��ȡ��Ĭ�ϼ��ػ�ַ
			pvModulePreferredBaseAddr = (PVOID)inth.OptionalHeader.ImageBase;
		}
	}
	return(pvModulePreferredBaseAddr);
}

// ��װ����ʽ�����NtQueryInformationProcess
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

	NTSTATUS lStatus = -1;  // Ĭ�Ϸ���-1

	// �õ�NtQueryInformationProcess��������
	// ע�⣺dll�еĺ���������char
	PFN_NTQUERYINFORMATIONPROCESS pfnNtQIP =
		(PFN_NTQUERYINFORMATIONPROCESS)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if (pfnNtQIP != nullptr) {
		lStatus = pfnNtQIP(hProcess, pic, pPI, cbSize, pLength);
	}
	// �ͷŶ�̬��
	FreeLibrary(hNtDll);
	return(lStatus); // ����NTSTATUS�ṹ
}

// ͨ�����̾����ȡָ��������������Ϣ
BOOL ProcessInfo::GetProcessCmdLine(HANDLE hProcess, LPTSTR szCmdLine, DWORD Size) {

	// �������
	if ((hProcess == nullptr) || (szCmdLine == nullptr) || (Size == 0))
		return(FALSE);

	// 0. ����Ҫ��ý��̵�PEB��ַ
	DWORD dwSize;
	SIZE_T size;
	PROCESS_BASIC_INFORMATION  pbi;
	// ����PEB�ṹ��xpϵͳ��ʼ�����ڽ��������ַ�ռ��е�0x7ffdf000λ��
	// ����PEB�ĵ�ַ��ÿһ�汾Windows�����������
	// ��������õ�PEB����ȷ����
	// ��ͨ��NtQueryInformationProcess�����õ�PROCESS_BASIC_INFORMATION�ṹ
	// NtQueryInformationProcess����ʧ��ʱ�᷵�ظ���
	if (0 > _NtQueryInformationProcess(
		hProcess,
		ProcessBasicInformation,
		&pbi,
		sizeof(pbi),
		&dwSize)) {
		return FALSE;
	}

	// 1. ͨ��pbi�ṩ��PEB��ַ�ҵ����̻�����
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

	// 2. ��PEB�ṹ�л�ȡָ�����������������Ϣ�Ľṹ��ָ��
	__INFOBLOCK Block;
	if (!ReadProcessMemory(
		hProcess,
		(LPVOID)PEB.InfoBlockAddress,
		&Block,
		sizeof(Block),
		&size)) {
		return(FALSE);
	}

	// 3. ��ȡ��������Ϣ ע�� ����Unicodeȥ�ӵ�
	wchar_t wszCmdLine[MAX_PATH + 1];
	if (!ReadProcessMemory(
		hProcess,
		(LPVOID)Block.wszCmdLineAddress,
		wszCmdLine,
		MAX_PATH * sizeof(wchar_t),
		&size)) {
		return(FALSE);
	}

	// 4. ������������Ϣ�е��ļ�·����һ�� ���п����ǿ��ַ���
	//    �ļ�·���п�������������ʽ"c:\...\app.exe" �� c:\...\app.exe
	wchar_t* pPos = wszCmdLine; // �õ���һ�����ַ�λ��
	// �ڵ�һ���ַ����ǿ��ַ��������
	if (*pPos != L'\0') {
		if (*pPos == L'"') {
			// �����˫������ʽ �Ͷ�λ����һ��˫����
			pPos = wcschr(&pPos[1], L'"');
		}
		else {
			// Find the next SPACE character
			// ��Ȼ����û��˫���ŵ���ʽ ֱ�Ӷ�λ���ո�
			pPos = wcschr(&pPos[1], L' ');
		}

		// ������ ��ʱ���Ѿ��������в���������
		if (pPos != nullptr) // ������ʵ�Ǽ��������wcschr�Ƿ�ִ�гɹ�
			pPos++;
	}

	// ���Ѿ������ļ�·������������Ϣ���ƻ�ȥ
	if (pPos != nullptr && *pPos != L'\0') {
		// ������ڵĻ�
		// ����PEB����Ϣ��Unicode����
		// ���� �������ı�����ʽ��ͬ �ͷ���������ʽ
#ifdef UNICODE
		_tcscpy_s(szCmdLine, Size, pPos);
#else
		MultiByteToWideChar(CP_ACP, 0, szCmdLine, Size,
			pPos, wcslen(pPos));
#endif
	}
	else
		szCmdLine[0] = TEXT('\0');
	// ���ж�û���� ����true
	return(TRUE);
}

// ͨ��pid��ȡָ��������������Ϣ
BOOL ProcessInfo::GetProcessCmdLine(DWORD PID, LPTSTR szCmdLine, DWORD Size) {

	// �������
	if ((PID <= 0) || (szCmdLine == nullptr))
		return(FALSE);

	// ��ָ�����̶��� ���Ƿ���Ȩ�޲鿴
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
	if (hProcess == nullptr)
		return(FALSE);
	// ����· ִ������汾�ĺ���
	BOOL bReturn = GetProcessCmdLine(hProcess, szCmdLine, Size);

	// �ͷž��
	CloseHandle(hProcess);

	return(bReturn);
}

// ͨ�����̾����ȡ����������
BOOL ProcessInfo::GetProcessOwner(HANDLE hProcess, LPTSTR szOwner, size_t cchSize) {

	// �������
	if ((szOwner == nullptr) || (cchSize == 0))
		return(FALSE);

	// ����Ĭ��ֵ
	szOwner[0] = TEXT('\0');

	// �õ��������ƾ��
	HANDLE hToken = nullptr;
	// ������һ���Ƚϸߵ�Ȩ�ޣ���Ϊ����ϵͳ��һ���� �Բ���ϵͳ��ʽ������
	CToolhelp::EnablePrivilege(SE_TCB_NAME, TRUE);
	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
		CToolhelp::EnablePrivilege(SE_TCB_NAME, FALSE);
		return(FALSE);
	}

	// ��ȡTOKEN_USER�ṹ��С
	DWORD cbti = 0;
	GetTokenInformation(hToken, TokenUser, nullptr, 0, &cbti);

	// �����Ҹ�Ϊdo-while��ʽ �߼������һЩ
	do {
		// ���ڸղ�ֻ�������С ���Է����������ֵ����������
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			break;
		// ����ѿռ�����Žṹ��
		PTOKEN_USER ptiUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), 0, cbti);
		if (ptiUser == nullptr)
			break;

		// �õ�ָ��token��tokenuser��Ϣ
		if (GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti)) {
			SID_NAME_USE   snu;
			TCHAR          szUser[MAX_PATH];
			DWORD          chUser = MAX_PATH;
			PDWORD         pcchUser = &chUser;
			TCHAR          szDomain[MAX_PATH];
			DWORD          chDomain = MAX_PATH;
			PDWORD         pcchDomain = &chDomain;

			// ͨ���û�SID��ȡ�û�������������
			if (LookupAccountSid(
				nullptr,
				ptiUser->User.Sid,
				szUser,
				pcchUser,
				szDomain,
				pcchDomain,
				&snu)) {
				// ����\\DomainName\UserName��ʽ���ַ���
				_tcscpy_s(szOwner, cchSize, TEXT("\\\\"));
				_tcscat_s(szOwner, cchSize, szDomain);
				_tcscat_s(szOwner, cchSize, TEXT("\\"));
				_tcscat_s(szOwner, cchSize, szUser);
			}
		}

		// �ͷ��ڴ�
		HeapFree(GetProcessHeap(), 0, ptiUser);

	} while (FALSE);

	// �ͷž��
	CloseHandle(hToken);

	// ����Ȩ������
	CToolhelp::EnablePrivilege(SE_TCB_NAME, FALSE);

	return(TRUE);
}

// ͨ������pid��ȡ����������
BOOL ProcessInfo::GetProcessOwner(DWORD PID, LPTSTR szOwner, DWORD cchSize) {

	// �������
	if ((PID <= 0) || (szOwner == nullptr))
		return(FALSE);

	// �򿪽��̾��
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);
	if (hProcess == nullptr)
		return(FALSE);

	// ���ǵ��������Ǹ�����
	BOOL bReturn = GetProcessOwner(hProcess, szOwner, cchSize);

	// �ͷž��
	CloseHandle(hProcess);

	return(bReturn);
}

// ������СΪKB��λ
VOID ProcessInfo::FormatSizeInKB(
	DWORD dwSize,
	DWORD nCharacters,
	LPTSTR szSize,
	size_t cchSize) {

	TCHAR szFormattedSize[64];
	// ת��ʧ�ܾ�û�취�ˡ�������
	if (StrFormatKBSize(dwSize, szFormattedSize, _countof(szFormattedSize)) == nullptr)
		StringCchPrintf(szFormattedSize, _countof(szFormattedSize), TEXT("%8u"), dwSize);

	// �����ɵ��ַ�����Ҫ�����ұ���
	if (_tcslen(szFormattedSize) < nCharacters) {
		// ��ǰ������ʵ������Ŀո�
		DWORD current = 0;
		for (current = 0;
			current < (nCharacters - _tcslen(szFormattedSize));
			current++) {
			szSize[current] = TEXT(' ');
		}
		szSize[current] = TEXT('\0'); // Ҫ��Ϊ0��β���ַ��� ���ܵ��������ƴ�Ӻ���
		_tcscat_s(szSize, cchSize - current, szFormattedSize);
	}
	else {
		_tcscpy_s(szSize, cchSize, szFormattedSize);
	}
}

// ����pid��textedit��ʾָ��������Ϣ
VOID ProcessInfo::ShowProcessInfo(DWORD dwProcessID) {

	ui.plainTextEdit->setPlainText(""); // �����ʾ��

	CToolhelp th(TH32CS_SNAPALL, dwProcessID);

	// ��ʾ���̵���Ϣ
	// ͨ���������̿��� �õ�PROCESSENTRY32�ṹ
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = th.ProcessFirst(&pe);
	for (; fOk; fOk = th.ProcessNext(&pe)) {
		if (pe.th32ProcessID == dwProcessID) {
			// ��ʾ��������Ϣ
			TCHAR szCmdLine[1024];
			if (GetProcessCmdLine(dwProcessID, szCmdLine, _countof(szCmdLine))) {
				AddText(TEXT("Command line: %s %s\r\n"), pe.szExeFile, szCmdLine);
			}
			else {
				AddText(TEXT("Filename: %s\r\n"), pe.szExeFile);
			}
			// ��ʾϸ����Ϣ
			AddText(
				TEXT("   PID=%08X, ParentPID=%08X, PriorityClass=%d, Threads=%d, Heaps=%d\r\n"),
				pe.th32ProcessID,
				pe.th32ParentProcessID,
				pe.pcPriClassBase,
				pe.cntThreads,
				th.HowManyHeaps());
			// ��ʾ����������
			TCHAR szOwner[MAX_PATH + 1];
			if (GetProcessOwner(dwProcessID, szOwner, MAX_PATH)) {
				AddText(TEXT("Owner: %s\r\n"), szOwner);
			}

			break;   // �ҵ�ָ�����̲���ʾ����Ϣ�������
		}
	}


	// ��ʾ�����е�ģ����Ϣ
	// �ѱ���д��
	AddText(
		TEXT("\r\nModules Information:\r\n  Usage  %-*s(%-*s)  %10s  Module\r\n"),
		s_cchAddress, TEXT("BaseAddr"), // �������ǰ�s_cchAddress�������
		s_cchAddress, TEXT("ImagAddr"),
		TEXT("Size"));
	// ����ģ����� �������ģ����Ϣ
	// ������CToolhelp�жԺ��������˷�װ
	// ��ʼ������֮�� �������ֱ�ӱ����ý��̵�ģ��
	MODULEENTRY32 me = { sizeof(me) };
	fOk = th.ModuleFirst(&me);
	for (; fOk; fOk = th.ModuleNext(&me)) {
		// ��ӡ��ģ���ʹ�ü���
		if (me.ProccntUsage == 65535) {
			// ���ʾģ������ʽ����� ���Ҳ��ܱ�ж��
			AddText(TEXT("  Fixed"));
		}
		else {
			AddText(TEXT("  %5d"), me.ProccntUsage);
		}
		// ��ģ���С��λ��ΪKB
		TCHAR szFormattedSize[64];
		if (StrFormatKBSize(me.modBaseSize, szFormattedSize, _countof(szFormattedSize)) == nullptr)
		{
			StringCchPrintf(szFormattedSize, _countof(szFormattedSize), TEXT("%10u"), me.modBaseSize);
		}
		// ��ȡģ��Ĭ�ϼ��ػ�ַ ����ӡ��ȡ����ģ����Ϣ
		PVOID pvPreferredBaseAddr = GetModulePreferredBaseAddr(pe.th32ProcessID, me.modBaseAddr);
		if (me.modBaseAddr == pvPreferredBaseAddr) {
			AddText(
				TEXT("  %p %*s   %10s  %s\r\n"),
				me.modBaseAddr,
				s_cchAddress, TEXT(""), // ���ʾ��s_cchAddress������Ҷ���
				szFormattedSize,
				me.szExePath);
		}
		else {
			// ģ��û�м��ص�Ĭ�ϻ�ַ �Ͱ�Ĭ�ϼ��ص�ַ��ʵ�ʼ��ص�ַ��д��
			AddText(
				TEXT("  %p(%p)  %10s  %s\r\n"),
				me.modBaseAddr,
				pvPreferredBaseAddr,
				szFormattedSize,
				me.szExePath);
		}
	}

	// ��ʾ�ý����е��߳�
	AddText(TEXT("\r\nThread Information:\r\n      TID     Priority\r\n"));
	THREADENTRY32 te = { sizeof(te) };
	fOk = th.ThreadFirst(&te);
	for (; fOk; fOk = th.ThreadNext(&te)) {
		if (te.th32OwnerProcessID == dwProcessID) {
			// ������������ȼ�
			int nPriority = te.tpBasePri + te.tpDeltaPri;
			if ((te.tpBasePri < 16) && (nPriority > 15)) nPriority = 15;
			if ((te.tpBasePri > 15) && (nPriority > 31)) nPriority = 31;
			if ((te.tpBasePri < 16) && (nPriority < 1)) nPriority = 1;
			if ((te.tpBasePri > 15) && (nPriority < 16)) nPriority = 16;
			AddText(TEXT("   %08X       %2d\r\n"), te.th32ThreadID, nPriority);
		}
	}
	// ������Ƶ���ͷ
	ui.plainTextEdit->moveCursor(QTextCursor::Start);
}

// ����ģ��·�� ��ʾָ��ģ����Ϣ
VOID ProcessInfo::ShowModuleInfo(PCTSTR pszModulePath) {

	ui.plainTextEdit->setPlainText(""); // ���textedit
	// �ѱ���д��
	AddText(TEXT("Pathname: %s\r\n\r\n"), pszModulePath);
	AddText(TEXT("Process Information:\r\n"));
	AddText(
		TEXT("     PID    %-*s  Process\r\n"),
		s_cchAddress, TEXT("BaseAddr")); // ��s_cchAddres�������

// ͨ�����ռ��� ˫��ѭ���������н����е�����ģ�� �ҵ�ָ������һ��
	CToolhelp thProcesses(TH32CS_SNAPPROCESS);
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk = thProcesses.ProcessFirst(&pe);
	// ����ѭ��
	for (; fOk; fOk = thProcesses.ProcessNext(&pe)) {
		CToolhelp thModules(TH32CS_SNAPMODULE, pe.th32ProcessID);
		MODULEENTRY32 me = { sizeof(me) };
		BOOL fOk = thModules.ModuleFirst(&me);
		// ��Ӧ�����еĿ���ѭ��
		for (; fOk; fOk = thModules.ModuleNext(&me)) {
			// �Ƚ�
			if (_tcscmp(me.szExePath, pszModulePath) == 0) {
				AddText(TEXT("  %08X  %p  %s\r\n"),
					pe.th32ProcessID,
					me.modBaseAddr,
					pe.szExeFile);
			}
		}
	}
	// ������Ƶ���ͷ
	ui.plainTextEdit->moveCursor(QTextCursor::Start);
}

// �жϵ�ǰ�û��Ƿ��ǹ���Ա ����ȡ��ǰ����������Ϣ
BOOL ProcessInfo::GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin) {

	HANDLE hToken = nullptr;
	DWORD dwSize;

	// ���õ��Լ�������
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return(FALSE);

	BOOL bResult = FALSE;

	// ��ȡ����������Ϣ
	if (GetTokenInformation(
		hToken,
		TokenElevationType,
		pElevationType, // Ϊ�˽������ö��ֵ
		sizeof(TOKEN_ELEVATION_TYPE),
		&dwSize)) {
		// ����һ����������Ա���ͨ��SID
		byte adminSID[SECURITY_MAX_SID_SIZE];
		dwSize = sizeof(adminSID);
		CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, &adminSID, &dwSize);

		// �жϵ�ǰ���������Ƿ�ɸѡ��
		if (*pElevationType == TokenElevationTypeLimited) {
			// Get handle to linked token (will have one if we are lua)
			// ��ǰ�Ǳ�ɸѡ������ ��ô���õ��������������δ��ɸѡ��������Ϣ
			HANDLE hUnfilteredToken = nullptr;
			GetTokenInformation(
				hToken,
				TokenLinkedToken,
				(VOID*)& hUnfilteredToken,
				sizeof(HANDLE),
				&dwSize);

			// ��������Ƿ���һ������Ա�˺�SID
			if (CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin)) {
				bResult = TRUE;
			}

			// �رվ��
			CloseHandle(hUnfilteredToken);
		}
		else {
			// �����ǰ���Ǳ�ɸѡ���������� ֱ���жϾͿ���֪����ǰ�û��ǲ��ǹ�·Ա��
			*pIsAdmin = IsUserAnAdmin();
			bResult = TRUE;
		}
	}

	// �رվ��
	CloseHandle(hToken);

	return(bResult);
}

// һЩ��ʼ������
VOID ProcessInfo::Init() {
	// ��������жϵ�ǰ���Թ���Ա������г��� ������ΪTRUE
	BOOL bCanReadSystemProcesses = FALSE;

	// ���������Ƿ�����ɸѡ����������
	if (GetProcessElevation(&s_elevationType, &s_bIsAdmin)) {
		// prefix title with elevation
		// �����������������滻�Ĵ��ڱ���ǰ׺
		TCHAR szTitle[64];

		switch (s_elevationType) {
			// Ĭ���û�������UAC���󱻾ܾ�
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

			// ������Ȩ�ɹ�
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

			// ������ɸѡȨ��������
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

		// �����ڱ������Ϊ��Ӧ�û�ģʽ
		this->setWindowTitle(QString::fromWCharArray(szTitle) + this->windowTitle());

		// ����Ѿ��ǹ���Աģʽ���ǾͲ���Ҫ���UAC����ť��
		if (bCanReadSystemProcesses) {
			ui.actionSystem_processes->setEnabled(false);
		}
	}
	// ��ʾ����ģʽ
	ui.label->setText("Process mode");
	// �����ȼ��ؽ���comboBox
	PopulateProcessList();
}

// �����û���� �Թ���ԱȨ������ָ������
DWORD ProcessInfo::StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine) {

	// ��ʼ���ṹ
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = szExecutable;
	sei.lpParameters = szCmdLine;
	sei.nShow = SW_SHOWNORMAL; // ��һ�������� ��Ȼû�д�����ʾ

	ShellExecuteEx(&sei);
	return(GetLastError());
}

