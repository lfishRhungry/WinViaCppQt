/********************************************************************************
** Form generated from reading UI file 'Summation.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUMMATION_H
#define UI_SUMMATION_H

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

class Ui_SummationClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *labelRequest;
    QSpacerItem *horizontalSpacer;
    QLineEdit *lineEdit;
    QFrame *line;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonCalc;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelAnswer;
    QLabel *labelOutput;

    void setupUi(QWidget *SummationClass)
    {
        if (SummationClass->objectName().isEmpty())
            SummationClass->setObjectName(QString::fromUtf8("SummationClass"));
        SummationClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        SummationClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Summation.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SummationClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(SummationClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(SummationClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelRequest = new QLabel(widget);
        labelRequest->setObjectName(QString::fromUtf8("labelRequest"));

        horizontalLayout->addWidget(labelRequest);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addWidget(widget);

        line = new QFrame(SummationClass);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        widget_2 = new QWidget(SummationClass);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        buttonCalc = new QPushButton(widget_2);
        buttonCalc->setObjectName(QString::fromUtf8("buttonCalc"));

        horizontalLayout_2->addWidget(buttonCalc);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        labelAnswer = new QLabel(widget_2);
        labelAnswer->setObjectName(QString::fromUtf8("labelAnswer"));

        horizontalLayout_2->addWidget(labelAnswer);

        labelOutput = new QLabel(widget_2);
        labelOutput->setObjectName(QString::fromUtf8("labelOutput"));

        horizontalLayout_2->addWidget(labelOutput);


        verticalLayout->addWidget(widget_2);


        retranslateUi(SummationClass);

        QMetaObject::connectSlotsByName(SummationClass);
    } // setupUi

    void retranslateUi(QWidget *SummationClass)
    {
        SummationClass->setWindowTitle(QCoreApplication::translate("SummationClass", "Summation", nullptr));
        labelRequest->setText(QCoreApplication::translate("SummationClass", "TextLabel", nullptr));
        buttonCalc->setText(QCoreApplication::translate("SummationClass", "PushButton", nullptr));
        labelAnswer->setText(QCoreApplication::translate("SummationClass", "TextLabel", nullptr));
        labelOutput->setText(QCoreApplication::translate("SummationClass", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SummationClass: public Ui_SummationClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUMMATION_H
