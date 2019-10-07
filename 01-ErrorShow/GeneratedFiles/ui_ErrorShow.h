/********************************************************************************
** Form generated from reading UI file 'ErrorShow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERRORSHOW_H
#define UI_ERRORSHOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ErrorShowClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QCheckBox *checkBox;
    QPlainTextEdit *plainTextEdit;

    void setupUi(QWidget *ErrorShowClass)
    {
        if (ErrorShowClass->objectName().isEmpty())
            ErrorShowClass->setObjectName(QString::fromUtf8("ErrorShowClass"));
        ErrorShowClass->resize(648, 295);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        ErrorShowClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ErrorShow.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ErrorShowClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(ErrorShowClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(ErrorShowClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy1);
        lineEdit->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(lineEdit);

        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        pushButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        checkBox = new QCheckBox(widget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        horizontalLayout->addWidget(checkBox);


        verticalLayout->addWidget(widget);

        plainTextEdit = new QPlainTextEdit(ErrorShowClass);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);


        retranslateUi(ErrorShowClass);

        QMetaObject::connectSlotsByName(ErrorShowClass);
    } // setupUi

    void retranslateUi(QWidget *ErrorShowClass)
    {
        ErrorShowClass->setWindowTitle(QCoreApplication::translate("ErrorShowClass", "Error Show", nullptr));
        label->setText(QCoreApplication::translate("ErrorShowClass", "Error:", nullptr));
        pushButton->setText(QCoreApplication::translate("ErrorShowClass", "Look up", nullptr));
        checkBox->setText(QCoreApplication::translate("ErrorShowClass", "On top", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ErrorShowClass: public Ui_ErrorShowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERRORSHOW_H
