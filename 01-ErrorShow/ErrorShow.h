/*
通过格式化消息机制 将错误码转换为
当前系统语言描述并显示
*/

#pragma once

#include <QtWidgets/QWidget>
#include "ui_ErrorShow.h"
#include "../CommonFiles/CmnHdr.h"
#include <QCheckBox>
#include <QIntValidator>

class ErrorShow : public QWidget
{
	Q_OBJECT

public:
	ErrorShow(QWidget *parent = Q_NULLPTR);

private:
	Ui::ErrorShowClass ui;
};
