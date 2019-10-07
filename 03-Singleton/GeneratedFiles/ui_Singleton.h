/********************************************************************************
** Form generated from reading UI file 'Singleton.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SINGLETON_H
#define UI_SINGLETON_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SingletonClass
{
public:
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton;

    void setupUi(QWidget *SingletonClass)
    {
        if (SingletonClass->objectName().isEmpty())
            SingletonClass->setObjectName(QString::fromUtf8("SingletonClass"));
        SingletonClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        SingletonClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Singleton.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SingletonClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(SingletonClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        plainTextEdit = new QPlainTextEdit(SingletonClass);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        pushButton = new QPushButton(SingletonClass);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);


        retranslateUi(SingletonClass);

        QMetaObject::connectSlotsByName(SingletonClass);
    } // setupUi

    void retranslateUi(QWidget *SingletonClass)
    {
        SingletonClass->setWindowTitle(QCoreApplication::translate("SingletonClass", "Singleton", nullptr));
        pushButton->setText(QCoreApplication::translate("SingletonClass", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SingletonClass: public Ui_SingletonClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SINGLETON_H
