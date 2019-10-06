/********************************************************************************
** Form generated from reading UI file 'LastMsgBoxInfo.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LASTMSGBOXINFO_H
#define UI_LASTMSGBOXINFO_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LastMsgBoxInfoClass
{
public:
    QGridLayout *gridLayout;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QWidget *LastMsgBoxInfoClass)
    {
        if (LastMsgBoxInfoClass->objectName().isEmpty())
            LastMsgBoxInfoClass->setObjectName(QString::fromUtf8("LastMsgBoxInfoClass"));
        LastMsgBoxInfoClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        LastMsgBoxInfoClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/LastMsgBoxInfo.ico"), QSize(), QIcon::Normal, QIcon::Off);
        LastMsgBoxInfoClass->setWindowIcon(icon);
        gridLayout = new QGridLayout(LastMsgBoxInfoClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        plainTextEdit = new QPlainTextEdit(LastMsgBoxInfoClass);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        gridLayout->addWidget(plainTextEdit, 0, 0, 1, 1);


        retranslateUi(LastMsgBoxInfoClass);

        QMetaObject::connectSlotsByName(LastMsgBoxInfoClass);
    } // setupUi

    void retranslateUi(QWidget *LastMsgBoxInfoClass)
    {
        LastMsgBoxInfoClass->setWindowTitle(QCoreApplication::translate("LastMsgBoxInfoClass", "LastMsgBoxInfo", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LastMsgBoxInfoClass: public Ui_LastMsgBoxInfoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LASTMSGBOXINFO_H
