/*64λ�汾
  ���ڵ���LastMsgBoxInfoLib.dll
  ʹ��ȫ�ֹ���ע��ķ�ʽ ��messagebox��������IAT hook*/

#pragma once

#include <QtWidgets/QWidget>
#include "ui_LastMsgBoxInfox64.h"

class LastMsgBoxInfox64 : public QWidget
{
	Q_OBJECT

public:
	LastMsgBoxInfox64(QWidget *parent = Q_NULLPTR);
	~LastMsgBoxInfox64();
	// ��дnativeEvent�����Դ���windows������Ϣ
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::LastMsgBoxInfox64Class ui;
};
