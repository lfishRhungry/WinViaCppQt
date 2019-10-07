/********************************************************************************
** Form generated from reading UI file 'ProcessInfo.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSINFO_H
#define UI_PROCESSINFO_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProcessInfoClass
{
public:
    QAction *actionProcess_mode;
    QAction *actionModule_mode;
    QAction *actionVMmap_tool;
    QAction *actionSystem_processes;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QComboBox *comboBox;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menuBar;
    QMenu *menuProcesses;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ProcessInfoClass)
    {
        if (ProcessInfoClass->objectName().isEmpty())
            ProcessInfoClass->setObjectName(QString::fromUtf8("ProcessInfoClass"));
        ProcessInfoClass->resize(600, 400);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(10);
        ProcessInfoClass->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ProcessInfo.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ProcessInfoClass->setWindowIcon(icon);
        actionProcess_mode = new QAction(ProcessInfoClass);
        actionProcess_mode->setObjectName(QString::fromUtf8("actionProcess_mode"));
        actionModule_mode = new QAction(ProcessInfoClass);
        actionModule_mode->setObjectName(QString::fromUtf8("actionModule_mode"));
        actionVMmap_tool = new QAction(ProcessInfoClass);
        actionVMmap_tool->setObjectName(QString::fromUtf8("actionVMmap_tool"));
        actionSystem_processes = new QAction(ProcessInfoClass);
        actionSystem_processes->setObjectName(QString::fromUtf8("actionSystem_processes"));
        centralWidget = new QWidget(ProcessInfoClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Consolas"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);

        verticalLayout->addWidget(label);

        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        comboBox = new QComboBox(widget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout->addWidget(comboBox);


        verticalLayout->addWidget(widget);

        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        verticalLayout->addWidget(plainTextEdit);

        ProcessInfoClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ProcessInfoClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 22));
        menuProcesses = new QMenu(menuBar);
        menuProcesses->setObjectName(QString::fromUtf8("menuProcesses"));
        ProcessInfoClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ProcessInfoClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ProcessInfoClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ProcessInfoClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ProcessInfoClass->setStatusBar(statusBar);

        menuBar->addAction(menuProcesses->menuAction());
        menuProcesses->addAction(actionProcess_mode);
        menuProcesses->addAction(actionModule_mode);
        menuProcesses->addSeparator();
        menuProcesses->addAction(actionVMmap_tool);
        menuProcesses->addSeparator();
        menuProcesses->addAction(actionSystem_processes);

        retranslateUi(ProcessInfoClass);

        QMetaObject::connectSlotsByName(ProcessInfoClass);
    } // setupUi

    void retranslateUi(QMainWindow *ProcessInfoClass)
    {
        ProcessInfoClass->setWindowTitle(QCoreApplication::translate("ProcessInfoClass", "ProcessInfo", nullptr));
        actionProcess_mode->setText(QCoreApplication::translate("ProcessInfoClass", "Process mode", nullptr));
        actionModule_mode->setText(QCoreApplication::translate("ProcessInfoClass", "Module mode", nullptr));
        actionVMmap_tool->setText(QCoreApplication::translate("ProcessInfoClass", "VMmap tool", nullptr));
        actionSystem_processes->setText(QCoreApplication::translate("ProcessInfoClass", "System processes", nullptr));
        label->setText(QCoreApplication::translate("ProcessInfoClass", "TextLabel", nullptr));
        menuProcesses->setTitle(QCoreApplication::translate("ProcessInfoClass", "Menu", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProcessInfoClass: public Ui_ProcessInfoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSINFO_H
