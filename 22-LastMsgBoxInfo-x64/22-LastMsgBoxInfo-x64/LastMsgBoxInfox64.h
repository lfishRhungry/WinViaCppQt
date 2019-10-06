/*64位版本
  用于调用LastMsgBoxInfoLib.dll
  使用全局钩子注入的方式 对messagebox函数进行IAT hook*/

#pragma once

#include <QtWidgets/QWidget>
#include "ui_LastMsgBoxInfox64.h"

class LastMsgBoxInfox64 : public QWidget
{
	Q_OBJECT

public:
	LastMsgBoxInfox64(QWidget *parent = Q_NULLPTR);
	~LastMsgBoxInfox64();
	// 重写nativeEvent函数以处理windows窗口消息
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::LastMsgBoxInfox64Class ui;
};
