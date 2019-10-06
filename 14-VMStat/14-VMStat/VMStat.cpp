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
	// 先调用显示 不然等到定时器需要1秒钟
	timeDisplay();
	// 利用qtimer定时器类实现定时器功能
	QTimer* timer = new QTimer(this);
	timer->start(1000); // 每间隔一秒钟启动一次定时器
	connect(timer, &QTimer::timeout, [=]() {
		timeDisplay();
		});
}

// 画好标题
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

// 每次刷新时所完成的显示内容
void VMStat::timeDisplay() {
	// 调用GlobalMemoryStatus函数前初始化结构
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
