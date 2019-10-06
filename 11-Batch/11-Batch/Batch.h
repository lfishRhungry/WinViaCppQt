/*演示了使用windows线程池中添加
  异步线程函数的用法*/
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

// 自定义消息
#define WM_APP_COMPLETED (WM_APP+123)



class Batch : public QWidget
{
	Q_OBJECT

public:
	Batch(QWidget *parent = Q_NULLPTR);
	~Batch();
	// Windows消息处理函数
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);
	PTP_WORK g_pWorkItem = NULL; // 线程池

private:
	Ui::BatchClass ui;
};
