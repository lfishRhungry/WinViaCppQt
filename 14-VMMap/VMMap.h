/*通过在命令行键入进程ID
  遍历指定进程虚拟地址空间
  并显示对应信息
  如果没有在命令行进入指定进程ID
  就遍历自身虚拟地址空间xinxi*/
#pragma once

#include <QtWidgets/QWidget>
#include <QClipBoard>
#include <QMessageBox>
#include "ui_VMMap.h"
#include "../CommonFiles/CmnHdr.h"
#include "../CommonFiles/Toolhelp.h"
#include <psapi.h>
#include <windowsx.h>
#include <tchar.h>
#include "VMQuery.h"
#include <strsafe.h>

#pragma comment(lib, "psapi")

class VMMap : public QWidget
{
	Q_OBJECT


public:
	VMMap(QWidget *parent = Q_NULLPTR);
	// 将存储类型二进制信息转换为字符串
	PCTSTR GetMemStorageText(DWORD dwStorage);
	// 将保护属性二进制信息转换为字符串
	PTSTR GetProtectText(
		DWORD dwProtect, 
		PTSTR szBuf, 
		size_t chSize,
		BOOL bShowFlags);
	// 根据结构内容构造区域信息
	void ConstructRgnInfoLine(
		HANDLE hProcess, 
		PVMQUERY pVMQ,
		PTSTR szLine, 
		int cchMaxLen);
	// 根据结构内容构造块信息
	void ConstructBlkInfoLine(PVMQUERY pVMQ, PTSTR szLine, int cchMaxLen);
	// Refresh逻辑
	void Refresh(DWORD dwProcessId, BOOL bExpandRegions);
	// 初始化工作
	BOOL OnInit();

private:
	Ui::VMMapClass ui;
	DWORD g_dwProcessId = 0;  // 需要遍历的进程ID
	BOOL  g_bExpandRegions = FALSE;
	CToolhelp g_toolhelp;
};
