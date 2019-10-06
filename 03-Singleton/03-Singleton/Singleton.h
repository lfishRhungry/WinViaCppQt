/*
ͨ������һ��˽�������ռ� �������д���������
ʵ�ֲ�ѯ�����Ƿ�һ���е�Ч��
������Ҫ����һ�����ڹ���Ա�û���������ռ�
���Գ�����Ҫ�����ڹ���Ա�û���
*/

#pragma once

#include <QtWidgets/QWidget>
#include "ui_Singleton.h"
#include "../../CommonFiles/CmnHdr.h"
#include <sddl.h>
#include <strsafe.h>

class Singleton : public QWidget
{
	Q_OBJECT

public:
	Singleton(QWidget *parent = Q_NULLPTR);
	~Singleton();
	// ���߼�����
	void CheckInstance();

private:
	Ui::SingletonClass ui;
	// ���������߽����������������ռ��ȫ�־��
	HANDLE g_hSingleton = nullptr;
	HANDLE g_hBoundary = nullptr;
	HANDLE g_hNamespace = nullptr;
	// ��־ָ���������ռ��Ƿ��Ѿ�������
	BOOL g_bNamespaceOpened = FALSE;
	// ָ���߽���������˽�������ռ������
	PCTSTR g_szBoundary = TEXT("3-Boundary");
	PCTSTR g_szNamespace = TEXT("3-Namespace");
};
