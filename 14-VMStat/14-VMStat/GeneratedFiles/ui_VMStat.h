/********************************************************************************
** Form generated from reading UI file 'VMStat.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VMSTAT_H
#define UI_VMSTAT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VMStatClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QFrame *line;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;

    void setupUi(QWidget *VMStatClass)
    {
        if (VMStatClass->objectName().isEmpty())
            VMStatClass->setObjectName(QString::fromUtf8("VMStatClass"));
        VMStatClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        VMStatClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/VMStat.ico"), QSize(), QIcon::Normal, QIcon::Off);
        VMStatClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(VMStatClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(VMStatClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);


        verticalLayout->addWidget(widget);

        line = new QFrame(VMStatClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        widget_2 = new QWidget(VMStatClass);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_2->addWidget(label_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        label_4 = new QLabel(widget_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_2->addWidget(label_4);


        verticalLayout->addWidget(widget_2);


        retranslateUi(VMStatClass);

        QMetaObject::connectSlotsByName(VMStatClass);
    } // setupUi

    void retranslateUi(QWidget *VMStatClass)
    {
        VMStatClass->setWindowTitle(QCoreApplication::translate("VMStatClass", "VMStat", nullptr));
        label->setText(QCoreApplication::translate("VMStatClass", "TextLabel", nullptr));
        label_2->setText(QCoreApplication::translate("VMStatClass", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("VMStatClass", "TextLabel", nullptr));
        label_4->setText(QCoreApplication::translate("VMStatClass", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VMStatClass: public Ui_VMStatClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMSTAT_H
