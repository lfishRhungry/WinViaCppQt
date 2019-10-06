#include "VMStat.h"

VMStat::VMStat(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(500, 400);
	this->setWindowTitle("VMStat");
	ui.label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.label_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	ui.label_3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ui.label_4->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	titleDisplay();
	// �ȵ�����ʾ ��Ȼ�ȵ���ʱ����Ҫ1����
	timeDisplay();
	// ����qtimer��ʱ����ʵ�ֶ�ʱ������
	QTimer* timer = new QTimer(this);
	timer->start(1000); // ÿ���һ��������һ�ζ�ʱ��
	connect(timer, &QTimer::timeout, [=]() {
		timeDisplay();
		});
}

// ���ñ���
void VMStat::titleDisplay() {
	TCHAR szTitle[512] = { 0 };
	StringCchPrintf(szTitle, _countof(szTitle),
		TEXT("%s\n%s\n%s\n%s\n%s\n%s\n%s"),
		TEXT("Memory load:"),
		TEXT("TotalPhys:"),
		TEXT("AvailPhys:"),
		TEXT("TotalPageFile:"),
		TEXT("AvailPageFile:"),
		TEXT("TotalVirtual:"),
		TEXT("AvailVirtual:"));
	ui.label->setText(QString::fromWCharArray(szTitle));

	StringCchPrintf(szTitle, _countof(szTitle),
		TEXT("%s\n%s"),
		TEXT("WorkingSet:"),
		TEXT("PrivateBytes:"));
	ui.label_3->setText(QString::fromWCharArray(szTitle));
}

// ÿ��ˢ��ʱ����ɵ���ʾ����
void VMStat::timeDisplay() {
	// ����GlobalMemoryStatus����ǰ��ʼ���ṹ
	MEMORYSTATUS ms = { sizeof(ms) };
	GlobalMemoryStatus(&ms);

	TCHAR szData[512] = { 0 };
	StringCchPrintf(szData, _countof(szData),
		TEXT("%d\n%d\n%I64d\n%I64d\n%I64d\n%I64d\n%I64d"),
		ms.dwMemoryLoad, ms.dwTotalPhys,
		(__int64)ms.dwAvailPhys, (__int64)ms.dwTotalPageFile,
		(__int64)ms.dwAvailPageFile, (__int64)ms.dwTotalVirtual,
		(__int64)ms.dwAvailVirtual);

	ui.label_2->setText(QString::fromWCharArray(szData));

	// Get the current process working set and private bytes.
	PROCESS_MEMORY_COUNTERS_EX pmc = { sizeof(PROCESS_MEMORY_COUNTERS_EX) };
	GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
	StringCchPrintf(szData, _countof(szData),
		TEXT("%I64d K\n%I64d K"),
		(__int64)pmc.WorkingSetSize / 1024,
		(__int64)pmc.PrivateUsage / 1024);

	ui.label_4->setText(QString::fromWCharArray(szData));
}
