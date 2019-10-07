#pragma once

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include "ui_MMFShare.h"
#include "../CommonFiles/CmnHdr.h"
#include <windowsx.h>
#include <tchar.h>

class MMFShare : public QWidget
{
	Q_OBJECT

public:
	MMFShare(QWidget *parent = Q_NULLPTR);

private:
	Ui::MMFShareClass ui;
	// �򿪵��ڴ�ӳ���ļ��ں˶�����
	HANDLE m_hFileMap = NULL;
};
