#include "VMMap.h"

VMMap::VMMap(QWidget *parent)
	: QWidget(parent)
{
	
	// ����Ȩ��
	CToolhelp::EnablePrivilege(SE_DEBUG_NAME, TRUE);

	// ��ȡ�����в���
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

	// ������Ƶ�������
	connect(ui.copyButton, &QPushButton::clicked, [=]() {
		// ���ڼ�����
		QClipboard* clip = QApplication::clipboard();
		clip->setText(ui.plainTextEdit->toPlainText());
		});
	// Refresh��ť
	connect(ui.refreshButton, &QPushButton::clicked, [=]() {
		Refresh(g_dwProcessId, g_bExpandRegions);
		});
	// expand��ť
	connect(ui.expandButton, &QPushButton::clicked, [=]() {
		g_bExpandRegions = g_bExpandRegions ? FALSE : TRUE;
		Refresh(g_dwProcessId, g_bExpandRegions);
		});

	OnInit();
}

// ���洢���Ͷ�������Ϣת��Ϊ�ַ���
PCTSTR VMMap::GetMemStorageText(DWORD dwStorage) {
	PCTSTR p = TEXT("Unknown");
	switch (dwStorage) {
	case MEM_FREE:    p = TEXT("Free   "); break;
	case MEM_RESERVE: p = TEXT("Reserve"); break;
	case MEM_IMAGE:   p = TEXT("Image  "); break;
	case MEM_MAPPED:  p = TEXT("Mapped "); break;
	case MEM_PRIVATE: p = TEXT("Private"); break;
	}
	// ���˸о����������ַ�������ȫ
	// ����������龰�¶��Ǻ������غ�
	// ����ʹ���˷��ص��ַ��� Ӧ����û�����
	return(p);
}

// ���������Զ�������Ϣת��Ϊ�ַ���
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

// ���ݽṹ���ݹ���������Ϣ
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

// ���ݽṹ���ݹ������Ϣ
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

// Refresh�߼�
void VMMap::Refresh(DWORD dwProcessId, BOOL bExpandRegions) {

	// ��վ�����
	ui.plainTextEdit->clear();

	// ����ͨ��PIDָ�� ��Ҫ�ý��̻������вſ��Բ�ѯ�ڴ���Ϣ
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
		FALSE, dwProcessId);

	if (hProcess == NULL) {
		ui.plainTextEdit->setPlainText("\n    The process ID identifies a process that is not running");
		return;
	}

	// ��ʼ��ϲ���ּ����Ŀ��ջ
	g_toolhelp.CreateSnapshot(TH32CS_SNAPALL, dwProcessId);

	// ͨ��VMQuery���������ڴ���Ϣ������ӵ�plainTextEdit
	BOOL bOk = TRUE;
	PVOID pvAddress = NULL;

	while (bOk) {

		VMQUERY vmq;
		bOk = VMQuery(hProcess, pvAddress, &vmq);

		if (bOk) {
			// ����Ҫ��ʾ��ÿ�����ݲ���ӵ�plainTextEdit
			TCHAR szLine[1024];
			ConstructRgnInfoLine(hProcess, &vmq, szLine, _countof(szLine));
			ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szLine));

			if (bExpandRegions) {
				for (DWORD dwBlock = 0; bOk && (dwBlock < vmq.dwRgnBlocks);
					dwBlock++) {

					ConstructBlkInfoLine(&vmq, szLine, _countof(szLine));
					ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szLine));

					// ��ȡ��һ�����ַ
					pvAddress = ((PBYTE)pvAddress + vmq.BlkSize);
					if (dwBlock < vmq.dwRgnBlocks - 1) {
						// �������һ����֮��ĵ�ַ�Ͳ�Ҫ�ٲ�ѯ��
						bOk = VMQuery(hProcess, pvAddress, &vmq);
					}
				}
			}

			// ��ȡ��һ���������ַ����ѯ
			pvAddress = ((PBYTE)vmq.pvRgnBaseAddress + vmq.RgnSize);
		}
	}
	CloseHandle(hProcess);
	// ������ƶ�����ǰ
	ui.plainTextEdit->moveCursor(QTextCursor::Start);
}

// ��ʼ������
BOOL VMMap::OnInit() {

	// �ڴ��ڱ�����ʾ����֤�ְ���ѯ�Ľ�����Ϣ
	TCHAR szCaption[MAX_PATH * 2];
	g_toolhelp.CreateSnapshot(TH32CS_SNAPALL, g_dwProcessId);
	PROCESSENTRY32 pe = { sizeof(pe) };
	StringCchPrintf(szCaption,
		_countof(szCaption), TEXT("Virtual Memory Map (PID=%u \"%s\")"),
		g_dwProcessId, g_toolhelp.ProcessFind(g_dwProcessId, &pe)
		? pe.szExeFile : TEXT("unknown"));
	this->setWindowTitle(QString::fromWCharArray(szCaption));

	// ��ˢ������
	Refresh(g_dwProcessId, g_bExpandRegions);
	return(TRUE);
}
