/*使用GlobalMemoryStatus函数来获取
系统当前虚拟内存信息并显示*/
#pragma once

#include <QtWidgets/QWidget>
#include <QTimer>
#include "ui_VMStat.h"
#include "../../CommonFiles/CmnHdr.h"
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <StrSafe.h>

#pragma comment (lib,"psapi.lib")

class VMStat : public QWidget
{
	Q_OBJECT

public:
	VMStat(QWidget *parent = Q_NULLPTR);
	// 每次刷新时所完成的显示内容
	void timeDisplay();
	// 画好标题
	void titleDisplay();

private:
	Ui::VMStatClass ui;
};
