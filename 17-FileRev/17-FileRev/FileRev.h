/*
����ָ���ı��ļ�����һ��
ͨ�����ļ�ӳ�䷽ʽ�򿪸���
ֱ���������ڴ�ռ��н������ݷ�ת
���൱����ʵ�ʲ��������ļ�
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
	// ��ת�ļ��ĺ���
	BOOL FileReverse(PCTSTR pszPathname, PBOOL pbIsTextUnicode);

private:
	Ui::FileRevClass ui;
	// �洢·��
	TCHAR szPathname[MAX_PATH];
};
