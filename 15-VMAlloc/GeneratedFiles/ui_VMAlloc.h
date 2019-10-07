/********************************************************************************
** Form generated from reading UI file 'VMAlloc.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VMALLOC_H
#define UI_VMALLOC_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VMAllocClass
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelPage;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonReserve;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelIndex;
    QLineEdit *lineEdit;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *buttonUse;
    QPushButton *buttonClear;
    QPushButton *buttonCollect;
    QFrame *line;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_0;
    QLabel *label_1;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *labelWhite;
    QLabel *labelGray;
    QLabel *labelBlack;

    void setupUi(QWidget *VMAllocClass)
    {
        if (VMAllocClass->objectName().isEmpty())
            VMAllocClass->setObjectName(QString::fromUtf8("VMAllocClass"));
        VMAllocClass->resize(1825, 522);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        VMAllocClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/VMAlloc.ico"), QSize(), QIcon::Normal, QIcon::Off);
        VMAllocClass->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(VMAllocClass);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        widget_3 = new QWidget(VMAllocClass);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelPage = new QLabel(widget_3);
        labelPage->setObjectName(QString::fromUtf8("labelPage"));

        horizontalLayout_3->addWidget(labelPage);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        buttonReserve = new QPushButton(widget_3);
        buttonReserve->setObjectName(QString::fromUtf8("buttonReserve"));

        horizontalLayout_3->addWidget(buttonReserve);


        verticalLayout_2->addWidget(widget_3);

        widget_4 = new QWidget(VMAllocClass);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelIndex = new QLabel(widget_4);
        labelIndex->setObjectName(QString::fromUtf8("labelIndex"));

        horizontalLayout_4->addWidget(labelIndex);

        lineEdit = new QLineEdit(widget_4);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_4->addWidget(lineEdit);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        buttonUse = new QPushButton(widget_4);
        buttonUse->setObjectName(QString::fromUtf8("buttonUse"));

        horizontalLayout_4->addWidget(buttonUse);

        buttonClear = new QPushButton(widget_4);
        buttonClear->setObjectName(QString::fromUtf8("buttonClear"));

        horizontalLayout_4->addWidget(buttonClear);

        buttonCollect = new QPushButton(widget_4);
        buttonCollect->setObjectName(QString::fromUtf8("buttonCollect"));

        horizontalLayout_4->addWidget(buttonCollect);


        verticalLayout_2->addWidget(widget_4);

        line = new QFrame(VMAllocClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line);

        groupBox = new QGroupBox(VMAllocClass);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget_2 = new QWidget(groupBox);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_0 = new QLabel(widget_2);
        label_0->setObjectName(QString::fromUtf8("label_0"));

        horizontalLayout_2->addWidget(label_0);

        label_1 = new QLabel(widget_2);
        label_1->setObjectName(QString::fromUtf8("label_1"));

        horizontalLayout_2->addWidget(label_1);

        label_2 = new QLabel(widget_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_2->addWidget(label_3);

        label_4 = new QLabel(widget_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_2->addWidget(label_4);

        label_5 = new QLabel(widget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_2->addWidget(label_5);

        label_6 = new QLabel(widget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_2->addWidget(label_6);

        label_7 = new QLabel(widget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_2->addWidget(label_7);

        label_8 = new QLabel(widget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_2->addWidget(label_8);

        label_9 = new QLabel(widget_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_2->addWidget(label_9);

        label_10 = new QLabel(widget_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_2->addWidget(label_10);

        label_11 = new QLabel(widget_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_2->addWidget(label_11);

        label_12 = new QLabel(widget_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_2->addWidget(label_12);

        label_13 = new QLabel(widget_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_2->addWidget(label_13);

        label_14 = new QLabel(widget_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_2->addWidget(label_14);

        label_15 = new QLabel(widget_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_2->addWidget(label_15);

        label_16 = new QLabel(widget_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_2->addWidget(label_16);

        label_17 = new QLabel(widget_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        horizontalLayout_2->addWidget(label_17);

        label_18 = new QLabel(widget_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        horizontalLayout_2->addWidget(label_18);

        label_19 = new QLabel(widget_2);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        horizontalLayout_2->addWidget(label_19);

        label_20 = new QLabel(widget_2);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        horizontalLayout_2->addWidget(label_20);

        label_21 = new QLabel(widget_2);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        horizontalLayout_2->addWidget(label_21);

        label_22 = new QLabel(widget_2);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        horizontalLayout_2->addWidget(label_22);

        label_23 = new QLabel(widget_2);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        horizontalLayout_2->addWidget(label_23);

        label_24 = new QLabel(widget_2);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        horizontalLayout_2->addWidget(label_24);


        verticalLayout->addWidget(widget_2);

        widget = new QWidget(groupBox);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelWhite = new QLabel(widget);
        labelWhite->setObjectName(QString::fromUtf8("labelWhite"));

        horizontalLayout->addWidget(labelWhite);

        labelGray = new QLabel(widget);
        labelGray->setObjectName(QString::fromUtf8("labelGray"));

        horizontalLayout->addWidget(labelGray);

        labelBlack = new QLabel(widget);
        labelBlack->setObjectName(QString::fromUtf8("labelBlack"));

        horizontalLayout->addWidget(labelBlack);


        verticalLayout->addWidget(widget);


        verticalLayout_2->addWidget(groupBox);


        retranslateUi(VMAllocClass);

        QMetaObject::connectSlotsByName(VMAllocClass);
    } // setupUi

    void retranslateUi(QWidget *VMAllocClass)
    {
        VMAllocClass->setWindowTitle(QCoreApplication::translate("VMAllocClass", "VMAlloc", nullptr));
        labelPage->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        buttonReserve->setText(QCoreApplication::translate("VMAllocClass", "PushButton", nullptr));
        labelIndex->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        buttonUse->setText(QCoreApplication::translate("VMAllocClass", "PushButton", nullptr));
        buttonClear->setText(QCoreApplication::translate("VMAllocClass", "PushButton", nullptr));
        buttonCollect->setText(QCoreApplication::translate("VMAllocClass", "PushButton", nullptr));
        groupBox->setTitle(QCoreApplication::translate("VMAllocClass", "Memory Map", nullptr));
        label_0->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_1->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_2->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_4->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_5->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_6->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_7->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_8->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_9->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_10->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_11->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_12->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_13->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_14->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_15->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_16->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_17->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_18->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_19->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_20->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_21->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_22->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_23->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        label_24->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        labelWhite->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        labelGray->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
        labelBlack->setText(QCoreApplication::translate("VMAllocClass", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VMAllocClass: public Ui_VMAllocClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMALLOC_H
