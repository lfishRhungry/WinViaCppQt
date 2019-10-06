/********************************************************************************
** Form generated from reading UI file 'SysInfo.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYSINFO_H
#define UI_SYSINFO_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SysInfoClass
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *SysInfoClass)
    {
        if (SysInfoClass->objectName().isEmpty())
            SysInfoClass->setObjectName(QString::fromUtf8("SysInfoClass"));
        SysInfoClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        SysInfoClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/SysInfo.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SysInfoClass->setWindowIcon(icon);
        gridLayout = new QGridLayout(SysInfoClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tableWidget = new QTableWidget(SysInfoClass);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        gridLayout->addWidget(tableWidget, 0, 0, 1, 1);


        retranslateUi(SysInfoClass);

        QMetaObject::connectSlotsByName(SysInfoClass);
    } // setupUi

    void retranslateUi(QWidget *SysInfoClass)
    {
        SysInfoClass->setWindowTitle(QCoreApplication::translate("SysInfoClass", "SysInfo", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SysInfoClass: public Ui_SysInfoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYSINFO_H
