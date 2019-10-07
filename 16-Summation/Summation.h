/*
以计算指定数字的累加和为例
表现了单独开设线程时的线程栈溢出情景
演示了如何使用SEH和线程函数性质来优雅处理栈溢出
*/
#pragma once

#include <QtWidgets/QWidget>
#include <QIntValidator>
#include <QMessageBox>
#include "ui_Summation.h"
#include "../CommonFiles/CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <limits.h>
#include <tchar.h>

// 单独开线程计算累加和使用到的函数
UINT Sum(UINT uNum); // 递归计算函数
LONG WINAPI FilterFunc(DWORD dwExceptionCode); // 设置SEH相关 过滤函数
DWORD WINAPI SumThreadFunc(PVOID pvParam); // 线程调用函数


class Summation : public QWidget
{
	Q_OBJECT

public:
	Summation(QWidget *parent = Q_NULLPTR);

private:
	Ui::SummationClass ui;
};
