/*���ڵ���LastMsgBoxInfoLib.dll
  ʹ��ȫ�ֹ���ע��ķ�ʽ ��messagebox��������IAT hook*/
#pragma once

#include <QtWidgets/QWidget>
#include "ui_LastMsgBoxInfo.h"

class LastMsgBoxInfo : public QWidget
{
	Q_OBJECT

public:
	LastMsgBoxInfo(QWidget *parent = Q_NULLPTR);
	~LastMsgBoxInfo();
	// ��дnativeEvent�����Դ���windows������Ϣ
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::LastMsgBoxInfoClass ui;
};
