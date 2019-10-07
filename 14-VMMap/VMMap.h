/*ͨ���������м������ID
  ����ָ�����������ַ�ռ�
  ����ʾ��Ӧ��Ϣ
  ���û���������н���ָ������ID
  �ͱ������������ַ�ռ�xinxi*/
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
	// ���洢���Ͷ�������Ϣת��Ϊ�ַ���
	PCTSTR GetMemStorageText(DWORD dwStorage);
	// ���������Զ�������Ϣת��Ϊ�ַ���
	PTSTR GetProtectText(
		DWORD dwProtect, 
		PTSTR szBuf, 
		size_t chSize,
		BOOL bShowFlags);
	// ���ݽṹ���ݹ���������Ϣ
	void ConstructRgnInfoLine(
		HANDLE hProcess, 
		PVMQUERY pVMQ,
		PTSTR szLine, 
		int cchMaxLen);
	// ���ݽṹ���ݹ������Ϣ
	void ConstructBlkInfoLine(PVMQUERY pVMQ, PTSTR szLine, int cchMaxLen);
	// Refresh�߼�
	void Refresh(DWORD dwProcessId, BOOL bExpandRegions);
	// ��ʼ������
	BOOL OnInit();

private:
	Ui::VMMapClass ui;
	DWORD g_dwProcessId = 0;  // ��Ҫ�����Ľ���ID
	BOOL  g_bExpandRegions = FALSE;
	CToolhelp g_toolhelp;
};
