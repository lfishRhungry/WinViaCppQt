/*一个可以显示当前系统中进程信息及模块信息的程序
主要依靠了快照技术以及各种琐碎的形式来查找对应信息*/
#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessagebox>
#include "ui_ProcessInfo.h"
#include "../../CommonFiles/CmnHdr.h"
#include "../../CommonFiles/Toolhelp.h"
#include <stdarg.h>
#include <stdio.h>
#include <winternl.h>   // for Windows internal declarations.
#include <aclapi.h>     // for ACL management.
#include <shlwapi.h>    // for StrFormatKBSize.
#include <shlobj.h>     // for IsUserAnAdmin.
#include <tchar.h>
#include <strsafe.h>

// 告诉链接器把这几个库连接上
#pragma comment (lib,"shlwapi.lib")
#pragma comment (lib,"shell32.lib")

// 为了获取PEB结构
typedef struct
{
	DWORD Filler[4];
	DWORD InfoBlockAddress;
} __PEB;

typedef struct
{
	DWORD Filler[17];
	DWORD wszCmdLineAddress;
} __INFOBLOCK;

// 获取未显式导出的NtQueryInformationProcess函数 在winternl.h中被声明
// 主要是为了获取PEB结构 从而得到指定进程的命令行信息
typedef NTSTATUS(CALLBACK* PFN_NTQUERYINFORMATIONPROCESS)(
	HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength,
	PULONG ReturnLength OPTIONAL
	);

/*------------------------------------------------------------------------------------------------*/


class ProcessInfo : public QMainWindow
{
	Q_OBJECT

public:
	ProcessInfo(QWidget* parent = Q_NULLPTR);
	~ProcessInfo();
	// 在ComboBox显示进程
	VOID PopulateProcessList();
	// 在comboBox显示模块
	VOID PopulateModuleList();
	// 根据pid显示指定进程信息
	VOID ShowProcessInfo(DWORD dwProcessID);
	// 根据模块路径 显示指定模块信息
	VOID ShowModuleInfo(PCTSTR pszModulePath);
	// 判断当前用户是否是管理员 并获取当前进程提升信息
	BOOL GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin);
	// 一些初始化工作
	VOID Init();
	// 征求用户意见 以管理员权限启动指定程序
	DWORD StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine);

private:
	Ui::ProcessInfoClass ui;
	// 一些全局性静态变量
	TOKEN_ELEVATION_TYPE s_elevationType = TokenElevationTypeDefault; // 令牌权限提升的类型
	BOOL                 s_bIsAdmin = FALSE; // 是否是管理员用户
	const int			 s_cchAddress = sizeof(PVOID) * 2;

private:
	// 消解appendPlainText自动添加的换行符
	void appendPlainTextOnly(const QString& message);
	// 将参数按格式化字符串拼接之后追加输出到textedit面板
	void AddText(PCTSTR pszFormat, ...);

	// 根据进程句柄获取指定进程的完整性级别
	BOOL GetProcessIntegrityLevel(
		HANDLE hProcess,
		PDWORD pIntegrityLevel,
		PDWORD pPolicy,
		PDWORD pResourceIntegrityLevel,
		PDWORD pResourcePolicy);
	// 根据pid和系统完整性级别信息获取指定进程的完整性级别
	BOOL GetProcessIntegrityLevel(
		DWORD  PID,
		PDWORD pIntegrityLevel,
		PDWORD pPolicy,
		PDWORD pResourceIntegrityLevel,
		PDWORD pResourcePolicy);

	// 通过模块句柄和pid读取其PE头部信息获取模块默认加载基址
	PVOID GetModulePreferredBaseAddr(DWORD dwProcessId, PVOID pvModuleRemote);

	// 封装了显式导入的NtQueryInformationProcess
	NTSTATUS _NtQueryInformationProcess(HANDLE hProcess,
		PROCESSINFOCLASS pic,
		PVOID pPI,
		ULONG cbSize,
		PULONG pLength
	);

	// 通过进程句柄获取指定进程命令行信息
	BOOL GetProcessCmdLine(HANDLE hProcess, LPTSTR szCmdLine, DWORD Size);
	// 通过pid获取指定进程命令行信息
	BOOL GetProcessCmdLine(DWORD PID, LPTSTR szCmdLine, DWORD Size);

	// 通过进程句柄获取进程所有者
	BOOL GetProcessOwner(HANDLE hProcess, LPTSTR szOwner, size_t cchSize);
	// 通过进程pid获取进程所有者
	BOOL GetProcessOwner(DWORD PID, LPTSTR szOwner, DWORD cchSize);

	// 调整大小为KB单位
	VOID FormatSizeInKB(
		DWORD dwSize,
		DWORD nCharacters,
		LPTSTR szSize,
		size_t cchSize);

};
