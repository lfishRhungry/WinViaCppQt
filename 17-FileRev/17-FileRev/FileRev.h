/*
程序将指定文本文件复制一份
通过以文件映射方式打开副本
直接在虚拟内存空间中将其内容反转
就相当于在实际操作副本文件
*/
#pragma once

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include "ui_FileRev.h"
#include "../../CommonFiles/CmnHdr.h"
#include <windowsx.h>
#include <tchar.h>
#include <commdlg.h>
#include <string.h>

class FileRev : public QWidget
{
	Q_OBJECT

public:
	FileRev(QWidget *parent = Q_NULLPTR);
	// 反转文件的函数
	BOOL FileReverse(PCTSTR pszPathname, PBOOL pbIsTextUnicode);

private:
	Ui::FileRevClass ui;
	// 存储路径
	TCHAR szPathname[MAX_PATH];
};
