/*
���������һ���ڴ�ӳ���ļ���PE��
�����ڳ��������ù����ڴ�� �洢һ������
�ڳ���Ķ��ʵ���й��� �ﵽʶ���м�������ʵ����Ч��
����ʵ������ʱ�õ���windows��Ϣ���ͺʹ���
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
	// ��дnativeEvent�����Դ���windows������Ϣ
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::AppInstClass ui;
	// �㲥��Ϣʹ�õ���Ϣ��ʶ��
	UINT g_uMsgAppInstCountUpdate = WM_APP + 123;
};
