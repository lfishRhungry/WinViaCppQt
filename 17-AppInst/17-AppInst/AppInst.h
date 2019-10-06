/*
程序本身就是一个内存映射文件（PE）
于是在程序中设置共享内存段 存储一个变量
在程序的多个实例中共享 达到识别有几个程序实例的效果
更新实例数量时用到了windows消息发送和处理
*/
#pragma once

#include <QtWidgets/QWidget>
#include "ui_AppInst.h"
#include "../../CommonFiles/CmnHdr.h"
#include <windowsx.h>
#include <tchar.h>


class AppInst : public QWidget
{
	Q_OBJECT

public:
	AppInst(QWidget *parent = Q_NULLPTR);
	~AppInst();
	// 重写nativeEvent函数以处理windows窗口消息
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::AppInstClass ui;
	// 广播消息使用的消息标识号
	UINT g_uMsgAppInstCountUpdate = WM_APP + 123;
};
