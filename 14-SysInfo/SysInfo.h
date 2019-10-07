/*查询一些系统信息并以表格形式显示*/
#pragma once

#include <QtWidgets/QWidget>
#include "ui_SysInfo.h"
#include "../CommonFiles/CmnHdr.h"
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

class SysInfo : public QWidget
{
	Q_OBJECT

public:
	SysInfo(QWidget *parent = Q_NULLPTR);
	// 将数字转换为带分割逗号的字符串形式
	PTSTR BigNumToString(LONG lNum, PTSTR szBuf, DWORD chBufSize);
	// 根据GetSystemInfo函数的返回来显示系统信息
	void ShowCPUInfo(
		WORD wProcessorArchitecture, 
		WORD wProcessorLevel,
		WORD wProcessorRevision);
	// 根据程序位数和系统位数修改窗口标题
	void ShowBitness();

private:
	Ui::SysInfoClass ui;
};
