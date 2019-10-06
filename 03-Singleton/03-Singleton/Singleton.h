/*
通过创建一个私有命名空间 并在其中创建互斥量
实现查询程序是否单一运行的效果
由于需要创建一个属于管理员用户组的命名空间
所以程序需要运行在管理员用户下
*/

#pragma once

#include <QtWidgets/QWidget>
#include "ui_Singleton.h"
#include "../../CommonFiles/CmnHdr.h"
#include <sddl.h>
#include <strsafe.h>

class Singleton : public QWidget
{
	Q_OBJECT

public:
	Singleton(QWidget *parent = Q_NULLPTR);
	~Singleton();
	// 主逻辑函数
	void CheckInstance();

private:
	Ui::SingletonClass ui;
	// 互斥量、边界描述符对象、命名空间的全局句柄
	HANDLE g_hSingleton = nullptr;
	HANDLE g_hBoundary = nullptr;
	HANDLE g_hNamespace = nullptr;
	// 标志指定的命名空间是否已经被创建
	BOOL g_bNamespaceOpened = FALSE;
	// 指定边界描述符和私有命名空间的名称
	PCTSTR g_szBoundary = TEXT("3-Boundary");
	PCTSTR g_szNamespace = TEXT("3-Namespace");
};
