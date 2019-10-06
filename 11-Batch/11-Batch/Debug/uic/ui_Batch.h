/********************************************************************************
** Form generated from reading UI file 'Batch.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCH_H
#define UI_BATCH_H

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

class Ui_BatchClass
{
public:
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonStart;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *buttonExit;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *BatchClass)
    {
        if (BatchClass->objectName().isEmpty())
            BatchClass->setObjectName(QString::fromUtf8("BatchClass"));
        BatchClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        BatchClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Batch.ico"), QSize(), QIcon::Normal, QIcon::Off);
        BatchClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(BatchClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        plainTextEdit = new QPlainTextEdit(BatchClass);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        widget = new QWidget(BatchClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonStart = new QPushButton(widget);
        buttonStart->setObjectName(QString::fromUtf8("buttonStart"));

        horizontalLayout->addWidget(buttonStart);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        buttonExit = new QPushButton(widget);
        buttonExit->setObjectName(QString::fromUtf8("buttonExit"));

        horizontalLayout->addWidget(buttonExit);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(widget);


        retranslateUi(BatchClass);

        QMetaObject::connectSlotsByName(BatchClass);
    } // setupUi

    void retranslateUi(QWidget *BatchClass)
    {
        BatchClass->setWindowTitle(QCoreApplication::translate("BatchClass", "Batch", nullptr));
        buttonStart->setText(QCoreApplication::translate("BatchClass", "PushButton", nullptr));
        buttonExit->setText(QCoreApplication::translate("BatchClass", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BatchClass: public Ui_BatchClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCH_H
