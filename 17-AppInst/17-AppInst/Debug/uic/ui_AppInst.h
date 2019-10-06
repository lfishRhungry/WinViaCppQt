/********************************************************************************
** Form generated from reading UI file 'AppInst.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPINST_H
#define UI_APPINST_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppInstClass
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *labelContent;
    QLabel *labelNum;

    void setupUi(QWidget *AppInstClass)
    {
        if (AppInstClass->objectName().isEmpty())
            AppInstClass->setObjectName(QString::fromUtf8("AppInstClass"));
        AppInstClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        AppInstClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/AppInst.ico"), QSize(), QIcon::Normal, QIcon::Off);
        AppInstClass->setWindowIcon(icon);
        horizontalLayout = new QHBoxLayout(AppInstClass);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelContent = new QLabel(AppInstClass);
        labelContent->setObjectName(QString::fromUtf8("labelContent"));

        horizontalLayout->addWidget(labelContent);

        labelNum = new QLabel(AppInstClass);
        labelNum->setObjectName(QString::fromUtf8("labelNum"));

        horizontalLayout->addWidget(labelNum);


        retranslateUi(AppInstClass);

        QMetaObject::connectSlotsByName(AppInstClass);
    } // setupUi

    void retranslateUi(QWidget *AppInstClass)
    {
        AppInstClass->setWindowTitle(QCoreApplication::translate("AppInstClass", "AppInst", nullptr));
        labelContent->setText(QCoreApplication::translate("AppInstClass", "TextLabel", nullptr));
        labelNum->setText(QCoreApplication::translate("AppInstClass", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AppInstClass: public Ui_AppInstClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPINST_H
