#include "VMMap.h"

VMMap::VMMap(QWidget *parent)
	: QWidget(parent)
{
	
	// 提升权限
	CToolhelp::EnablePrivilege(SE_DEBUG_NAME, TRUE);

	// 获取命令行参数
	QStringList arguments = QCoreApplication::arguments();
	if (arguments.count() == 2) {
		g_dwProcessId = arguments.at(1).toUInt();
	}

	if (g_dwProcessId == 0) {
		g_dwProcessId = GetCurrentProcessId();
		QMessageBox::information(this,
			"VMMap",
			"No Process Id passed on command line. Defaulting to this process"
		);
	}

	ui.setupUi(this);

	this->setWindowTitle("VMMap");
	this->setMinimumSize(1500, 1000);

	ui.refreshButton->setFixedWidth(155);
	ui.expandButton->setFixedWidth(220);
	ui.copyButton->setFixedWidth(300);
	ui.refreshButton->setText("Refresh");
	ui.expandButton->setText("Expand Region");
	ui.copyButton->setText("Copy to Clipboard");

	ui.plainTextEdit->setReadOnly(true);
	ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap);

	// 点击复制到剪贴板
	connect(ui.copyButton, &QPushButton::clicked, [=]() {
		// 用于剪贴板
		QClipboard* clip = QApplication::clipboard();
		clip->setText(ui.plainTextEdit->toPlainText());
		});
	// Refresh按钮
	connect(ui.refreshButton, &QPushButton::clicked, [=]() {
		Refresh(g_dwProcessId, g_bExpandRegions);
		});
	// expand按钮
	connect(ui.expandButton, &QPushButton::clicked, [=]() {
		g_bExpandRegions = g_bExpandRegions ? FALSE : TRUE;
		Refresh(g_dwProcessId, g_bExpandRegions);
		});

	OnInit();
}

// 将存储类型二进制信息转换为字符串
PCTSTR VMMap::GetMemStorageText(DWORD dwStorage) {
	PCTSTR p = TEXT("Unknown");
	switch (dwStorage) {
	case MEM_FREE:    p = TEXT("Free   "); break;
	case MEM_RESERVE: p = TEXT("Reserve"); break;
	case MEM_IMAGE:   p = TEXT("Image  "); break;
	case MEM_MAPPED:  p = TEXT("Mapped "); break;
	case MEM_PRIVATE: p = TEXT("Private"); break;
	}
	// 个人感觉这样返回字符串不安全
	// 但是这里的情景下都是函数返回后
	// 立马使用了返回的字符串 应该是没问题的
	return(p);
}

// 将保护属性二进制信息转换为字符串
PTSTR VMMap::GetProtectText(
	DWORD dwProtect,
	PTSTR szBuf,
	size_t chSize,
	BOOL bShowFlags) {

	PCTSTR p = TEXT("Unknown");
	switch (dwProtect & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE)) {
	case PAGE_READONLY:          p = TEXT("-R--"); break;
	case PAGE_READWRITE:         p = TEXT("-RW-"); break;
	case PAGE_WRITECOPY:         p = TEXT("-RWC"); break;
	case PAGE_EXECUTE:           p = TEXT("E---"); break;
	case PAGE_EXECUTE_READ:      p = TEXT("ER--"); break;
	case PAGE_EXECUTE_READWRITE: p = TEXT("ERW-"); break;
	case PAGE_EXECUTE_WRITECOPY: p = TEXT("ERWC"); break;
	case PAGE_NOACCESS:          p = TEXT("----"); break;
	}
	_tcscpy_s(szBuf, chSize, p);
	if (bShowFlags) {
		_tcscat_s(szBuf, chSize, TEXT(" "));
		_tcscat_s(szBuf, chSize, (dwProtect & PAGE_GUARD)
			? TEXT("G") : TEXT("-"));
		_tcscat_s(szBuf, chSize, (dwProtect & PAGE_NOCACHE)
			? TEXT("N") : TEXT("-"));
		_tcscat_s(szBuf, chSize, (dwProtect & PAGE_WRITECOMBINE)
			? TEXT("W") : TEXT("-"));
	}
	return(szBuf);
}

// 根据结构内容构造区域信息
void VMMap::ConstructRgnInfoLine(
	HANDLE hProcess,
	PVMQUERY pVMQ,
	PTSTR szLine,
	int cchMaxLen) {

	StringCchPrintf(szLine, cchMaxLen, TEXT("%p\t%s\t%12u\t"),
		pVMQ->pvRgnBaseAddress,
		GetMemStorageText(pVMQ->dwRgnStorage),
		pVMQ->RgnSize);

	if (pVMQ->dwRgnStorage != MEM_FREE) {
		StringCchPrintf(_tcschr(szLine, 0), cchMaxLen - _tcslen(szLine),
			TEXT("%u\t"), pVMQ->dwRgnBlocks);
		GetProtectText(pVMQ->dwRgnProtection, _tcschr(szLine, 0),
			cchMaxLen - _tcslen(szLine), FALSE);
	}
	else {
		_tcscat_s(szLine, cchMaxLen, TEXT("\t"));
	}

	_tcscat_s(szLine, cchMaxLen, TEXT("\t"));

	// Try to obtain the module pathname for this region.
	if ((pVMQ->dwRgnStorage != MEM_FREE) && (pVMQ->pvRgnBaseAddress != NULL)) {

		MODULEENTRY32 me = { sizeof(me) };
		if (g_toolhelp.ModuleFind(pVMQ->pvRgnBaseAddress, &me)) {
			_tcscat_s(szLine, cchMaxLen, me.szExePath);
		}
		else {
			int cchLen = _tcslen(szLine);
			// This is not a module; see if it's a memory-mapped file
			DWORD dwLen = GetMappedFileName(hProcess,
				pVMQ->pvRgnBaseAddress, szLine + cchLen, cchMaxLen - cchLen);
			if (dwLen == 0) {
				// NOTE: GetMappedFileName modifies the string when it fails
				szLine[cchLen] = 0;
			}
		}
	}

	if (pVMQ->bRgnIsAStack) {
		_tcscat_s(szLine, cchMaxLen, TEXT("Thread Stack"));
	}
}

// 根据结构内容构造块信息
void VMMap::ConstructBlkInfoLine(PVMQUERY pVMQ, PTSTR szLine, int cchMaxLen) {

	_stprintf_s(szLine, cchMaxLen, TEXT("   %p\t%s\t%12u\t"),
		pVMQ->pvBlkBaseAddress,
		GetMemStorageText(pVMQ->dwBlkStorage),
		pVMQ->BlkSize);

	if (pVMQ->dwBlkStorage != MEM_FREE) {
		// add an empty cell for number of regions
		_tcscat_s(szLine, cchMaxLen, TEXT("\t"));
		GetProtectText(pVMQ->dwBlkProtection, _tcschr(szLine, 0),
			cchMaxLen - _tcslen(szLine), TRUE);
	}
	else {
		_tcscat_s(szLine, cchMaxLen, TEXT("\t"));
	}

	_tcscat_s(szLine, cchMaxLen, TEXT("\t"));
}

// Refresh逻辑
void VMMap::Refresh(DWORD dwProcessId, BOOL bExpandRegions) {

	// 清空旧内容
	ui.plainTextEdit->clear();

	// 由于通过PID指定 需要该进程还在运行才可以查询内存信息
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
		FALSE, dwProcessId);

	if (hProcess == NULL) {
		ui.plainTextEdit->setPlainText("\n    The process ID identifies a process that is not running");
		return;
	}

	// 开始了喜闻乐见的拍快照活动
	g_toolhelp.CreateSnapshot(TH32CS_SNAPALL, dwProcessId);

	// 通过VMQuery函数遍历内存信息，并添加到plainTextEdit
	BOOL bOk = TRUE;
	PVOID pvAddress = NULL;

	while (bOk) {

		VMQUERY vmq;
		bOk = VMQuery(hProcess, pvAddress, &vmq);

		if (bOk) {
			// 构造要显示的每行内容并添加到plainTextEdit
			TCHAR szLine[1024];
			ConstructRgnInfoLine(hProcess, &vmq, szLine, _countof(szLine));
			ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szLine));

			if (bExpandRegions) {
				for (DWORD dwBlock = 0; bOk && (dwBlock < vmq.dwRgnBlocks);
					dwBlock++) {

					ConstructBlkInfoLine(&vmq, szLine, _countof(szLine));
					ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szLine));

					// 获取下一个块地址
					pvAddress = ((PBYTE)pvAddress + vmq.BlkSize);
					if (dwBlock < vmq.dwRgnBlocks - 1) {
						// 到达最后一个块之后的地址就不要再查询了
						bOk = VMQuery(hProcess, pvAddress, &vmq);
					}
				}
			}

			// 获取下一个区域基地址来查询
			pvAddress = ((PBYTE)vmq.pvRgnBaseAddress + vmq.RgnSize);
		}
	}
	CloseHandle(hProcess);
	// 将光标移动到最前
	ui.plainTextEdit->moveCursor(QTextCursor::Start);
}

// 初始化工作
BOOL VMMap::OnInit() {

	// 在窗口标题显示我们证字啊查询的进程信息
	TCHAR szCaption[MAX_PATH * 2];
	g_toolhelp.CreateSnapshot(TH32CS_SNAPALL, g_dwProcessId);
	PROCESSENTRY32 pe = { sizeof(pe) };
	StringCchPrintf(szCaption,
		_countof(szCaption), TEXT("Virtual Memory Map (PID=%u \"%s\")"),
		g_dwProcessId, g_toolhelp.ProcessFind(g_dwProcessId, &pe)
		? pe.szExeFile : TEXT("unknown"));
	this->setWindowTitle(QString::fromWCharArray(szCaption));

	// 先刷新内容
	Refresh(g_dwProcessId, g_bExpandRegions);
	return(TRUE);
}
