/********************************************************************************
** Form generated from reading UI file 'InjLibx64.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INJLIBX64_H
#define UI_INJLIBX64_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InjLibx64Class
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonChoose;
    QLineEdit *lineEditPath;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPid;
    QLineEdit *lineEditPid;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonInj;
    QPushButton *buttonElevate;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *InjLibx64Class)
    {
        if (InjLibx64Class->objectName().isEmpty())
            InjLibx64Class->setObjectName(QString::fromUtf8("InjLibx64Class"));
        InjLibx64Class->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        InjLibx64Class->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/InjLib.ico"), QSize(), QIcon::Normal, QIcon::Off);
        InjLibx64Class->setWindowIcon(icon);
        centralWidget = new QWidget(InjLibx64Class);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonChoose = new QPushButton(widget);
        buttonChoose->setObjectName(QString::fromUtf8("buttonChoose"));

        horizontalLayout->addWidget(buttonChoose);

        lineEditPath = new QLineEdit(widget);
        lineEditPath->setObjectName(QString::fromUtf8("lineEditPath"));

        horizontalLayout->addWidget(lineEditPath);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelPid = new QLabel(widget_2);
        labelPid->setObjectName(QString::fromUtf8("labelPid"));

        horizontalLayout_2->addWidget(labelPid);

        lineEditPid = new QLineEdit(widget_2);
        lineEditPid->setObjectName(QString::fromUtf8("lineEditPid"));

        horizontalLayout_2->addWidget(lineEditPid);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        buttonInj = new QPushButton(widget_2);
        buttonInj->setObjectName(QString::fromUtf8("buttonInj"));

        horizontalLayout_2->addWidget(buttonInj);

        buttonElevate = new QPushButton(widget_2);
        buttonElevate->setObjectName(QString::fromUtf8("buttonElevate"));

        horizontalLayout_2->addWidget(buttonElevate);


        verticalLayout->addWidget(widget_2);

        InjLibx64Class->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(InjLibx64Class);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 22));
        InjLibx64Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(InjLibx64Class);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        InjLibx64Class->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(InjLibx64Class);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        InjLibx64Class->setStatusBar(statusBar);

        retranslateUi(InjLibx64Class);

        QMetaObject::connectSlotsByName(InjLibx64Class);
    } // setupUi

    void retranslateUi(QMainWindow *InjLibx64Class)
    {
        InjLibx64Class->setWindowTitle(QCoreApplication::translate("InjLibx64Class", "InjLibx64", nullptr));
        buttonChoose->setText(QCoreApplication::translate("InjLibx64Class", "PushButton", nullptr));
        labelPid->setText(QCoreApplication::translate("InjLibx64Class", "TextLabel", nullptr));
        buttonInj->setText(QCoreApplication::translate("InjLibx64Class", "PushButton", nullptr));
        buttonElevate->setText(QCoreApplication::translate("InjLibx64Class", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InjLibx64Class: public Ui_InjLibx64Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INJLIBX64_H
