/*
一个对VitualAlloc函数进行实践的函数
假定了一个数据结构
为此数据结构数组在虚拟地址空间中预定相应页面
根据用户要求调拨页面 清除结构 取消调拨
实现过程中手动用label模拟了内存示意图
*/
#pragma once

#include <QtWidgets/QWidget>
#include "ui_VMAlloc.h"
#include <QIntValidator>
#include "../CommonFiles/CmnHdr.h"
#include <WindowsX.h>
#include <tchar.h>
#include <StrSafe.h>

// 结构数组数量
#define MAX_SOMEDATA    (50)

// 本例程用到的模拟数据结构(2KB大小)
typedef struct {
	BOOL bInUse;
	BYTE bOtherData[2048 - sizeof(BOOL)];
} SOMEDATA, * PSOMEDATA;

class VMAlloc : public QWidget
{
	Q_OBJECT

public:
	VMAlloc(QWidget* parent = Q_NULLPTR);
	// 这里主要是释放空间
	~VMAlloc();

private:
	Ui::VMAllocClass ui;
	// 本机页大小（KB）
	UINT g_uPageSize = 0;
	// Pointer to an array of data structures
	// 指向一个模拟数据结构数组
	PSOMEDATA g_pSomeData = NULL;
	// lineEdit的输入
	UINT g_uIndex = 0;
};
