/*ʹ��GlobalMemoryStatus��������ȡ
ϵͳ��ǰ�����ڴ���Ϣ����ʾ*/
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
	// ÿ��ˢ��ʱ����ɵ���ʾ����
	void timeDisplay();
	// ���ñ���
	void titleDisplay();

private:
	Ui::VMStatClass ui;
};
