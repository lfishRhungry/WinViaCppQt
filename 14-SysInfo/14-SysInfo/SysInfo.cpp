#include "SysInfo.h"

SysInfo::SysInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(1000, 600);
	// ���ñ��
	ui.tableWidget->setRowCount(9);
	ui.tableWidget->setColumnCount(1);
	// ����Ӧ���
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	// ��񲻿ɱ༭
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// �����Դ���ͷ
	ui.tableWidget->horizontalHeader()->setHidden(true);
	// ���ñ�ͷ���뷽ʽ
	ui.tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
	// ���ر����
	ui.tableWidget->setShowGrid(false);
	// ����ÿ�б���
	ui.tableWidget->setVerticalHeaderItem(0, new QTableWidgetItem("Processor Architecture:"));
	ui.tableWidget->setVerticalHeaderItem(1, new QTableWidgetItem("Processor level:"));
	ui.tableWidget->setVerticalHeaderItem(2, new QTableWidgetItem("Processor revision:"));
	ui.tableWidget->setVerticalHeaderItem(3, new QTableWidgetItem("Number of processors:"));
	ui.tableWidget->setVerticalHeaderItem(4, new QTableWidgetItem("Active processor mask:"));
	ui.tableWidget->setVerticalHeaderItem(5, new QTableWidgetItem("Allocation granularity:"));
	ui.tableWidget->setVerticalHeaderItem(6, new QTableWidgetItem("Page size:"));
	ui.tableWidget->setVerticalHeaderItem(7, new QTableWidgetItem("Minimum app. address:"));
	ui.tableWidget->setVerticalHeaderItem(8, new QTableWidgetItem("Maximum app. address:"));

	// �������߼�
	// ��ȡϵͳ��Ϣ
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);

	ShowCPUInfo(sinf.wProcessorArchitecture,
		sinf.wProcessorLevel, sinf.wProcessorRevision);

	TCHAR szBuf[50];
	ui.tableWidget->setItem(6, 0, 
		new QTableWidgetItem(QString::fromWCharArray(
			BigNumToString(sinf.dwPageSize, szBuf, _countof(szBuf)))));

	StringCchPrintf(szBuf, _countof(szBuf), TEXT("%p"),
		sinf.lpMinimumApplicationAddress);
	ui.tableWidget->setItem(7, 0, new QTableWidgetItem(QString::fromWCharArray(szBuf)));

	StringCchPrintf(szBuf, _countof(szBuf), TEXT("%p"),
		sinf.lpMaximumApplicationAddress);
	ui.tableWidget->setItem(8, 0, new QTableWidgetItem(QString::fromWCharArray(szBuf)));

	StringCchPrintf(szBuf, _countof(szBuf), TEXT("0x%016I64X"),
		(__int64)sinf.dwActiveProcessorMask);
	ui.tableWidget->setItem(4, 0, new QTableWidgetItem(QString::fromWCharArray(szBuf)));

	ui.tableWidget->setItem(3, 0, 
		new QTableWidgetItem(QString::fromWCharArray(
			BigNumToString(sinf.dwNumberOfProcessors, szBuf, _countof(szBuf)))));

	ui.tableWidget->setItem(5, 0, 
		new QTableWidgetItem(QString::fromWCharArray(
			BigNumToString(sinf.dwAllocationGranularity, szBuf, _countof(szBuf)))));

	// ���ñ�����뷽ʽ
	// ����Ҫ���úñ���֮��ſ��Ը��Ķ�������
	for (int i = 0; i < 9; i++) {
		ui.tableWidget->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	}
	// ���Ĵ��ڱ���
	ShowBitness();
}

// ������ת��Ϊ���ָ�ŵ��ַ�����ʽ
PTSTR SysInfo::BigNumToString(LONG lNum, PTSTR szBuf, DWORD chBufSize) {

	TCHAR szNum[100];
	StringCchPrintf(szNum, _countof(szNum), TEXT("%d"), lNum);
	NUMBERFMT nf;
	nf.NumDigits = 0; // С��λ����
	nf.LeadingZero = FALSE; // С����ǰ�Ƿ�����
	nf.Grouping = 3; // ����λ�������Ϊһ��
	nf.lpDecimalSep = TEXT("."); // С���ָ���
	nf.lpThousandSep = TEXT(","); // ǧλ�ָ���
	nf.NegativeOrder = 0; // �Ƿ���ʾ����
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNum, &nf, szBuf, chBufSize);
	return(szBuf);
}

// ����GetSystemInfo�����ķ�������ʾϵͳ��Ϣ
void SysInfo::ShowCPUInfo(
	WORD wProcessorArchitecture,
	WORD wProcessorLevel,
	WORD wProcessorRevision) {

	// Ԥ��Ĭ��ֵ
	TCHAR szCPUArch[64] = TEXT("(unknown)");
	TCHAR szCPULevel[64] = TEXT("(unknown)");
	TCHAR szCPURev[64] = TEXT("(unknown)");

	switch (wProcessorArchitecture) {
		// Notice that AMD processors are seen as PROCESSOR_ARCHITECTURE_INTEL.
		// In the Registry, the content of the "VendorIdentifier" key under 
		// HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\CentralProcessor\0
		// is either "GenuineIntel" or "AuthenticAMD"
	case PROCESSOR_ARCHITECTURE_INTEL:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("Intel"));
		switch (wProcessorLevel) {
		case 3: case 4:
			StringCchPrintf(szCPULevel, _countof(szCPULevel), TEXT("80%c86"), wProcessorLevel + '0');
			StringCchPrintf(szCPURev, _countof(szCPURev), TEXT("%c%d"),
				HIBYTE(wProcessorRevision) + TEXT('A'),
				LOBYTE(wProcessorRevision));
			break;

		case 5:
			_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium"));
			StringCchPrintf(szCPURev, _countof(szCPURev), TEXT("Model %d, Stepping %d"),
				HIBYTE(wProcessorRevision), LOBYTE(wProcessorRevision));
			break;

		case 6:
			switch (HIBYTE(wProcessorRevision)) { // Model
			case 1:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Pentium Pro"));
				break;

			case 3:
			case 5:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Pentium II"));
				break;

			case 6:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Celeron"));
				break;

			case 7:
			case 8:
			case 11:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Pentium III"));
				break;

			case 9:
			case 13:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Pentium M"));
				break;

			case 10:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Pentium Xeon"));
				break;

			case 15:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Core 2 Duo"));
				break;

			default:
				_tcscpy_s(szCPULevel, _countof(szCPULevel),
					TEXT("Unknown Pentium"));
				break;
			}
			StringCchPrintf(szCPURev, _countof(szCPURev), TEXT("Model %d, Stepping %d"),
				HIBYTE(wProcessorRevision), LOBYTE(wProcessorRevision));
			break;

		case 15:
			_tcscpy_s(szCPULevel, _countof(szCPULevel), TEXT("Pentium 4"));
			StringCchPrintf(szCPURev, _countof(szCPURev), TEXT("Model %d, Stepping %d"),
				HIBYTE(wProcessorRevision), LOBYTE(wProcessorRevision));
			break;
		}
		break;

	case PROCESSOR_ARCHITECTURE_IA64:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("IA-64"));
		StringCchPrintf(szCPULevel, _countof(szCPULevel), TEXT("%d"), wProcessorLevel);
		StringCchPrintf(szCPURev, _countof(szCPURev), TEXT("Model %c, Pass %d"),
			HIBYTE(wProcessorRevision) + TEXT('A'),
			LOBYTE(wProcessorRevision));
		break;

	case PROCESSOR_ARCHITECTURE_AMD64:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("AMD64"));
		StringCchPrintf(szCPULevel, _countof(szCPULevel), TEXT("%d"), wProcessorLevel);
		StringCchPrintf(szCPURev, _countof(szCPURev), TEXT("Model %c, Pass %d"),
			HIBYTE(wProcessorRevision) + TEXT('A'),
			LOBYTE(wProcessorRevision));
		break;


	case PROCESSOR_ARCHITECTURE_UNKNOWN:
	default:
		_tcscpy_s(szCPUArch, _countof(szCPUArch), TEXT("Unknown"));
		break;
	}
	//SetDlgItemText(hWnd, IDC_PROCARCH, szCPUArch);
	ui.tableWidget->setItem(0, 0, new QTableWidgetItem(QString::fromWCharArray(szCPUArch)));
	ui.tableWidget->setItem(1, 0, new QTableWidgetItem(QString::fromWCharArray(szCPULevel)));
	ui.tableWidget->setItem(2, 0, new QTableWidgetItem(QString::fromWCharArray(szCPURev)));
	//SetDlgItemText(hWnd, IDC_PROCLEVEL, szCPULevel);
	//SetDlgItemText(hWnd, IDC_PROCREV, szCPURev);
}

// ���ݳ���λ����ϵͳλ���޸Ĵ��ڱ���
void SysInfo::ShowBitness() {
	TCHAR szFullTitle[100];
	TCHAR szTitle[32];
	// Ԥ�����
	this->setWindowTitle("SysInfo");
	_tcscpy_s(szTitle, _countof(szTitle), TEXT("SysInfo"));

#if defined(_WIN64)
	// 64λ����ֻ����64λϵͳ����
	StringCchPrintf(szFullTitle, _countof(szFullTitle),
		TEXT("64-bit %s"), szTitle);
#else
	BOOL bIsWow64 = FALSE;
	if (!IsWow64Process(GetCurrentProcess(), &bIsWow64)) {
		chFAIL("Failed to get WOW64 state.");
		return;
	}

	if (bIsWow64) {
		StringCchPrintf(szFullTitle, _countof(szFullTitle),
			TEXT("32-bit %s on WOW64"), szTitle);
	}
	else {
		StringCchPrintf(szFullTitle, _countof(szFullTitle),
			TEXT("32-bit %s on 32-bit Windows"), szTitle);
	}
#endif

	this->setWindowTitle(QString::fromWCharArray(szFullTitle));
}
