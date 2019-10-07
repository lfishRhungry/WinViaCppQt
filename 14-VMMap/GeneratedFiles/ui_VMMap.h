/********************************************************************************
** Form generated from reading UI file 'VMMap.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VMMAP_H
#define UI_VMMAP_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VMMapClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *refreshButton;
    QPushButton *expandButton;
    QPushButton *copyButton;
    QSpacerItem *horizontalSpacer;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QWidget *VMMapClass)
    {
        if (VMMapClass->objectName().isEmpty())
            VMMapClass->setObjectName(QString::fromUtf8("VMMapClass"));
        VMMapClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        VMMapClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/VMMap.ico"), QSize(), QIcon::Normal, QIcon::Off);
        VMMapClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(VMMapClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(VMMapClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        refreshButton = new QPushButton(widget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));

        horizontalLayout->addWidget(refreshButton);

        expandButton = new QPushButton(widget);
        expandButton->setObjectName(QString::fromUtf8("expandButton"));

        horizontalLayout->addWidget(expandButton);

        copyButton = new QPushButton(widget);
        copyButton->setObjectName(QString::fromUtf8("copyButton"));

        horizontalLayout->addWidget(copyButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        plainTextEdit = new QPlainTextEdit(VMMapClass);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);


        retranslateUi(VMMapClass);

        QMetaObject::connectSlotsByName(VMMapClass);
    } // setupUi

    void retranslateUi(QWidget *VMMapClass)
    {
        VMMapClass->setWindowTitle(QCoreApplication::translate("VMMapClass", "VMMap", nullptr));
        refreshButton->setText(QCoreApplication::translate("VMMapClass", "PushButton", nullptr));
        expandButton->setText(QCoreApplication::translate("VMMapClass", "PushButton", nullptr));
        copyButton->setText(QCoreApplication::translate("VMMapClass", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VMMapClass: public Ui_VMMapClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMMAP_H
