/*
ʹ��Զ�߳�ע�루CreateRemoteThread��
��ʽע��dll
*/
#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_InjLibx64.h"
#include <QtWidgets/QWidget>
#include <QValidator>
#include <QFileDialog>
#include <QMessageBox>
#include "../../CommonFiles/CmnHdr.h"
#include "../../CommonFiles/Toolhelp.h" // �������������Ȩ�޺���
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>
#include <TlHelp32.h>
#include <StrSafe.h>

class InjLibx64 : public QMainWindow
{
	Q_OBJECT

public:
	InjLibx64(QWidget *parent = Q_NULLPTR);
	// Զ�߳�ע���߼�
	BOOL WINAPI InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile);

private:
	Ui::InjLibx64Class ui;
	// DLL·��
	PCTCH pFilePath = NULL;
};
