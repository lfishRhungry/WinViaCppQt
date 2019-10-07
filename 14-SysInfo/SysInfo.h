/*��ѯһЩϵͳ��Ϣ���Ա����ʽ��ʾ*/
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
	// ������ת��Ϊ���ָ�ŵ��ַ�����ʽ
	PTSTR BigNumToString(LONG lNum, PTSTR szBuf, DWORD chBufSize);
	// ����GetSystemInfo�����ķ�������ʾϵͳ��Ϣ
	void ShowCPUInfo(
		WORD wProcessorArchitecture, 
		WORD wProcessorLevel,
		WORD wProcessorRevision);
	// ���ݳ���λ����ϵͳλ���޸Ĵ��ڱ���
	void ShowBitness();

private:
	Ui::SysInfoClass ui;
};
