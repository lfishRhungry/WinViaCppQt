/********************************************************************************
** Form generated from reading UI file 'MMFShare.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MMFSHARE_H
#define UI_MMFSHARE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MMFShareClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonCreate;
    QPushButton *buttonClose;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelData;
    QLineEdit *lineEdit;
    QFrame *line;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *buttonMap;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *MMFShareClass)
    {
        if (MMFShareClass->objectName().isEmpty())
            MMFShareClass->setObjectName(QString::fromUtf8("MMFShareClass"));
        MMFShareClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        MMFShareClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/MMFShare.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MMFShareClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(MMFShareClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(MMFShareClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonCreate = new QPushButton(widget);
        buttonCreate->setObjectName(QString::fromUtf8("buttonCreate"));

        horizontalLayout->addWidget(buttonCreate);

        buttonClose = new QPushButton(widget);
        buttonClose->setObjectName(QString::fromUtf8("buttonClose"));

        horizontalLayout->addWidget(buttonClose);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(MMFShareClass);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelData = new QLabel(widget_2);
        labelData->setObjectName(QString::fromUtf8("labelData"));

        horizontalLayout_2->addWidget(labelData);

        lineEdit = new QLineEdit(widget_2);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout->addWidget(widget_2);

        line = new QFrame(MMFShareClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        widget_3 = new QWidget(MMFShareClass);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        buttonMap = new QPushButton(widget_3);
        buttonMap->setObjectName(QString::fromUtf8("buttonMap"));

        horizontalLayout_3->addWidget(buttonMap);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget_3);


        retranslateUi(MMFShareClass);

        QMetaObject::connectSlotsByName(MMFShareClass);
    } // setupUi

    void retranslateUi(QWidget *MMFShareClass)
    {
        MMFShareClass->setWindowTitle(QCoreApplication::translate("MMFShareClass", "MMFShare", nullptr));
        buttonCreate->setText(QCoreApplication::translate("MMFShareClass", "PushButton", nullptr));
        buttonClose->setText(QCoreApplication::translate("MMFShareClass", "PushButton", nullptr));
        labelData->setText(QCoreApplication::translate("MMFShareClass", "TextLabel", nullptr));
        buttonMap->setText(QCoreApplication::translate("MMFShareClass", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MMFShareClass: public Ui_MMFShareClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MMFSHARE_H
