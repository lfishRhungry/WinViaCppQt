/*
�Լ���ָ�����ֵ��ۼӺ�Ϊ��
�����˵��������߳�ʱ���߳�ջ����龰
��ʾ�����ʹ��SEH���̺߳������������Ŵ���ջ���
*/
#pragma once

#include <QtWidgets/QWidget>
#include <QIntValidator>
#include <QMessageBox>
#include "ui_Summation.h"
#include "../CommonFiles/CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <limits.h>
#include <tchar.h>

// �������̼߳����ۼӺ�ʹ�õ��ĺ���
UINT Sum(UINT uNum); // �ݹ���㺯��
LONG WINAPI FilterFunc(DWORD dwExceptionCode); // ����SEH��� ���˺���
DWORD WINAPI SumThreadFunc(PVOID pvParam); // �̵߳��ú���


class Summation : public QWidget
{
	Q_OBJECT

public:
	Summation(QWidget *parent = Q_NULLPTR);

private:
	Ui::SummationClass ui;
};
