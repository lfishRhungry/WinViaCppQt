/*��ʾ��ʹ��windows�̳߳������
  �첽�̺߳������÷�*/
#pragma once

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include "ui_Batch.h"
#include "../../CommonFiles/CmnHdr.h"
#include <Windowsx.h>
#include <WinBase.h>
#include <WinNT.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

// �Զ�����Ϣ
#define WM_APP_COMPLETED (WM_APP+123)



class Batch : public QWidget
{
	Q_OBJECT

public:
	Batch(QWidget *parent = Q_NULLPTR);
	~Batch();
	// Windows��Ϣ������
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);
	PTP_WORK g_pWorkItem = NULL; // �̳߳�

private:
	Ui::BatchClass ui;
};
