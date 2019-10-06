/*
使用远线程注入（CreateRemoteThread）
方式注入dll
*/
#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_InjLibx64.h"
#include <QtWidgets/QWidget>
#include <QValidator>
#include <QFileDialog>
#include <QMessageBox>
#include "../../CommonFiles/CmnHdr.h"
#include "../../CommonFiles/Toolhelp.h" // 用了里面的提升权限函数
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
	// 远线程注入逻辑
	BOOL WINAPI InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile);

private:
	Ui::InjLibx64Class ui;
	// DLL路径
	PCTCH pFilePath = NULL;
};
