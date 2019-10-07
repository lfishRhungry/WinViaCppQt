/*
һ����VitualAlloc��������ʵ���ĺ���
�ٶ���һ�����ݽṹ
Ϊ�����ݽṹ�����������ַ�ռ���Ԥ����Ӧҳ��
�����û�Ҫ�����ҳ�� ����ṹ ȡ������
ʵ�ֹ������ֶ���labelģ�����ڴ�ʾ��ͼ
*/
#pragma once

#include <QtWidgets/QWidget>
#include "ui_VMAlloc.h"
#include <QIntValidator>
#include "../CommonFiles/CmnHdr.h"
#include <WindowsX.h>
#include <tchar.h>
#include <StrSafe.h>

// �ṹ��������
#define MAX_SOMEDATA    (50)

// �������õ���ģ�����ݽṹ(2KB��С)
typedef struct {
	BOOL bInUse;
	BYTE bOtherData[2048 - sizeof(BOOL)];
} SOMEDATA, * PSOMEDATA;

class VMAlloc : public QWidget
{
	Q_OBJECT

public:
	VMAlloc(QWidget* parent = Q_NULLPTR);
	// ������Ҫ���ͷſռ�
	~VMAlloc();

private:
	Ui::VMAllocClass ui;
	// ����ҳ��С��KB��
	UINT g_uPageSize = 0;
	// Pointer to an array of data structures
	// ָ��һ��ģ�����ݽṹ����
	PSOMEDATA g_pSomeData = NULL;
	// lineEdit������
	UINT g_uIndex = 0;
};
