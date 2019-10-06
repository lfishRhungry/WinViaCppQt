/*һ��������ʾ��ǰϵͳ�н�����Ϣ��ģ����Ϣ�ĳ���
��Ҫ�����˿��ռ����Լ������������ʽ�����Ҷ�Ӧ��Ϣ*/
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

// �������������⼸����������
#pragma comment (lib,"shlwapi.lib")
#pragma comment (lib,"shell32.lib")

// Ϊ�˻�ȡPEB�ṹ
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

// ��ȡδ��ʽ������NtQueryInformationProcess���� ��winternl.h�б�����
// ��Ҫ��Ϊ�˻�ȡPEB�ṹ �Ӷ��õ�ָ�����̵���������Ϣ
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
	// ��ComboBox��ʾ����
	VOID PopulateProcessList();
	// ��comboBox��ʾģ��
	VOID PopulateModuleList();
	// ����pid��ʾָ��������Ϣ
	VOID ShowProcessInfo(DWORD dwProcessID);
	// ����ģ��·�� ��ʾָ��ģ����Ϣ
	VOID ShowModuleInfo(PCTSTR pszModulePath);
	// �жϵ�ǰ�û��Ƿ��ǹ���Ա ����ȡ��ǰ����������Ϣ
	BOOL GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin);
	// һЩ��ʼ������
	VOID Init();
	// �����û���� �Թ���ԱȨ������ָ������
	DWORD StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine);

private:
	Ui::ProcessInfoClass ui;
	// һЩȫ���Ծ�̬����
	TOKEN_ELEVATION_TYPE s_elevationType = TokenElevationTypeDefault; // ����Ȩ������������
	BOOL                 s_bIsAdmin = FALSE; // �Ƿ��ǹ���Ա�û�
	const int			 s_cchAddress = sizeof(PVOID) * 2;

private:
	// ����appendPlainText�Զ���ӵĻ��з�
	void appendPlainTextOnly(const QString& message);
	// ����������ʽ���ַ���ƴ��֮��׷�������textedit���
	void AddText(PCTSTR pszFormat, ...);

	// ���ݽ��̾����ȡָ�����̵������Լ���
	BOOL GetProcessIntegrityLevel(
		HANDLE hProcess,
		PDWORD pIntegrityLevel,
		PDWORD pPolicy,
		PDWORD pResourceIntegrityLevel,
		PDWORD pResourcePolicy);
	// ����pid��ϵͳ�����Լ�����Ϣ��ȡָ�����̵������Լ���
	BOOL GetProcessIntegrityLevel(
		DWORD  PID,
		PDWORD pIntegrityLevel,
		PDWORD pPolicy,
		PDWORD pResourceIntegrityLevel,
		PDWORD pResourcePolicy);

	// ͨ��ģ������pid��ȡ��PEͷ����Ϣ��ȡģ��Ĭ�ϼ��ػ�ַ
	PVOID GetModulePreferredBaseAddr(DWORD dwProcessId, PVOID pvModuleRemote);

	// ��װ����ʽ�����NtQueryInformationProcess
	NTSTATUS _NtQueryInformationProcess(HANDLE hProcess,
		PROCESSINFOCLASS pic,
		PVOID pPI,
		ULONG cbSize,
		PULONG pLength
	);

	// ͨ�����̾����ȡָ��������������Ϣ
	BOOL GetProcessCmdLine(HANDLE hProcess, LPTSTR szCmdLine, DWORD Size);
	// ͨ��pid��ȡָ��������������Ϣ
	BOOL GetProcessCmdLine(DWORD PID, LPTSTR szCmdLine, DWORD Size);

	// ͨ�����̾����ȡ����������
	BOOL GetProcessOwner(HANDLE hProcess, LPTSTR szOwner, size_t cchSize);
	// ͨ������pid��ȡ����������
	BOOL GetProcessOwner(DWORD PID, LPTSTR szOwner, DWORD cchSize);

	// ������СΪKB��λ
	VOID FormatSizeInKB(
		DWORD dwSize,
		DWORD nCharacters,
		LPTSTR szSize,
		size_t cchSize);

};
