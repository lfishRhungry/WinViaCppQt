/********************************************************************************
** Form generated from reading UI file 'FileRev.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEREV_H
#define UI_FILEREV_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileRevClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPath;
    QLineEdit *lineEdit;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonBrowse;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonReverse;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelChar;
    QLabel *labelType;

    void setupUi(QWidget *FileRevClass)
    {
        if (FileRevClass->objectName().isEmpty())
            FileRevClass->setObjectName(QString::fromUtf8("FileRevClass"));
        FileRevClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        FileRevClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/FileRev.ico"), QSize(), QIcon::Normal, QIcon::Off);
        FileRevClass->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(FileRevClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(FileRevClass);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelPath = new QLabel(widget);
        labelPath->setObjectName(QString::fromUtf8("labelPath"));

        horizontalLayout_2->addWidget(labelPath);

        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(FileRevClass);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonBrowse = new QPushButton(widget_2);
        buttonBrowse->setObjectName(QString::fromUtf8("buttonBrowse"));

        horizontalLayout->addWidget(buttonBrowse);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonReverse = new QPushButton(widget_2);
        buttonReverse->setObjectName(QString::fromUtf8("buttonReverse"));

        horizontalLayout->addWidget(buttonReverse);


        verticalLayout->addWidget(widget_2);

        widget_3 = new QWidget(FileRevClass);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelChar = new QLabel(widget_3);
        labelChar->setObjectName(QString::fromUtf8("labelChar"));

        horizontalLayout_3->addWidget(labelChar);

        labelType = new QLabel(widget_3);
        labelType->setObjectName(QString::fromUtf8("labelType"));

        horizontalLayout_3->addWidget(labelType);


        verticalLayout->addWidget(widget_3);


        retranslateUi(FileRevClass);

        QMetaObject::connectSlotsByName(FileRevClass);
    } // setupUi

    void retranslateUi(QWidget *FileRevClass)
    {
        FileRevClass->setWindowTitle(QCoreApplication::translate("FileRevClass", "FileRev", nullptr));
        labelPath->setText(QCoreApplication::translate("FileRevClass", "TextLabel", nullptr));
        buttonBrowse->setText(QCoreApplication::translate("FileRevClass", "PushButton", nullptr));
        buttonReverse->setText(QCoreApplication::translate("FileRevClass", "PushButton", nullptr));
        labelChar->setText(QCoreApplication::translate("FileRevClass", "TextLabel", nullptr));
        labelType->setText(QCoreApplication::translate("FileRevClass", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FileRevClass: public Ui_FileRevClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEREV_H
