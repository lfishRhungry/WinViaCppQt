/*用于调用LastMsgBoxInfoLib.dll
  使用全局钩子注入的方式 对messagebox函数进行IAT hook*/
#pragma once

#include <QtWidgets/QWidget>
#include "ui_LastMsgBoxInfo.h"

class LastMsgBoxInfo : public QWidget
{
	Q_OBJECT

public:
	LastMsgBoxInfo(QWidget *parent = Q_NULLPTR);
	~LastMsgBoxInfo();
	// 重写nativeEvent函数以处理windows窗口消息
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::LastMsgBoxInfoClass ui;
};
