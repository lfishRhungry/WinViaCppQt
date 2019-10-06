/********************************************************************************
** Form generated from reading UI file 'LastMsgBoxInfox64.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LASTMSGBOXINFOX64_H
#define UI_LASTMSGBOXINFOX64_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LastMsgBoxInfox64Class
{
public:
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QWidget *LastMsgBoxInfox64Class)
    {
        if (LastMsgBoxInfox64Class->objectName().isEmpty())
            LastMsgBoxInfox64Class->setObjectName(QString::fromUtf8("LastMsgBoxInfox64Class"));
        LastMsgBoxInfox64Class->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        LastMsgBoxInfox64Class->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/LastMsgBoxInfo.ico"), QSize(), QIcon::Normal, QIcon::Off);
        LastMsgBoxInfox64Class->setWindowIcon(icon);
        gridLayout = new QGridLayout(LastMsgBoxInfox64Class);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        plainTextEdit = new QPlainTextEdit(LastMsgBoxInfox64Class);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        gridLayout->addWidget(plainTextEdit, 0, 0, 1, 1);


        retranslateUi(LastMsgBoxInfox64Class);

        QMetaObject::connectSlotsByName(LastMsgBoxInfox64Class);
    } // setupUi

    void retranslateUi(QWidget *LastMsgBoxInfox64Class)
    {
        LastMsgBoxInfox64Class->setWindowTitle(QCoreApplication::translate("LastMsgBoxInfox64Class", "LastMsgBoxInfox64", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LastMsgBoxInfox64Class: public Ui_LastMsgBoxInfox64Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LASTMSGBOXINFOX64_H
