// 22-LastMsgBoxInfoLib.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "22-LastMsgBoxInfoLib.h"
#include "../CommonFiles/CmnHdr.h"
#include <WindowsX.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include "APIHook.h"

///////////////////////////////////////////////////////////////////////////////


// 要hook的函数原型
typedef int (WINAPI* PFNMESSAGEBOXA)(HWND hWnd, PCSTR pszText,
	PCSTR pszCaption, UINT uType);

typedef int (WINAPI* PFNMESSAGEBOXW)(HWND hWnd, PCWSTR pszText,
	PCWSTR pszCaption, UINT uType);


// 这里声明为全局变量(是声明不是定义 所以无参数)
extern CAPIHook g_MessageBoxA;
extern CAPIHook g_MessageBoxW;


///////////////////////////////////////////////////////////////////////////////


// 用来给我们的主调用对话框发消息通知
void SendLastMsgBoxInfo(BOOL bUnicode,
	PVOID pvCaption, PVOID pvText, int nResult) {

	// 调用MessageBox进程的路径名
	wchar_t szProcessPathname[MAX_PATH];
	GetModuleFileNameW(NULL, szProcessPathname, MAX_PATH);

	// 将返回值转换为可读性强的字符串
	PCWSTR pszResult = L"(Unknown)";
	switch (nResult) {
	case IDOK:       pszResult = L"Ok";        break;
	case IDCANCEL:   pszResult = L"Cancel";    break;
	case IDABORT:    pszResult = L"Abort";     break;
	case IDRETRY:    pszResult = L"Retry";     break;
	case IDIGNORE:   pszResult = L"Ignore";    break;
	case IDYES:      pszResult = L"Yes";       break;
	case IDNO:       pszResult = L"No";        break;
	case IDCLOSE:    pszResult = L"Close";     break;
	case IDHELP:     pszResult = L"Help";      break;
	case IDTRYAGAIN: pszResult = L"Try Again"; break;
	case IDCONTINUE: pszResult = L"Continue";  break;
	}

	// 构造要发送的内容
	wchar_t sz[2048];
	// 相信有的人和我一样 看了很久下面的三目表达式
	// 在这里告诉大家 首先 不同点在于%s和%S
	// 其次 区别在于在这里输出宽字符函数还是单字节
	StringCchPrintfW(sz, _countof(sz), bUnicode
		? L"Process: (%d) %s\r\nCaption: %s\r\nMessage: %s\r\nResult: %s"
		: L"Process: (%d) %s\r\nCaption: %S\r\nMessage: %S\r\nResult: %s",
		GetCurrentProcessId(), szProcessPathname,
		pvCaption, pvText, pszResult);

	// 发送FORWARD_WM_COPYDATA消息 下面那个函数是Windowsx.h中的一个方便的宏
	COPYDATASTRUCT cds = { 0, ((DWORD)wcslen(sz) + 1) * sizeof(wchar_t), sz };
	FORWARD_WM_COPYDATA(FindWindow(NULL, TEXT("Last MessageBox Info")),
		NULL, &cds, SendMessage);
}


///////////////////////////////////////////////////////////////////////////////

// MessageBoxW 替代函数
int WINAPI Hook_MessageBoxW(HWND hWnd, PCWSTR pszText, LPCWSTR pszCaption,
	UINT uType) {

	// 调用原函数(注意用到了一个重写的PROC()操作符)
	int nResult = ((PFNMESSAGEBOXW)(PROC)g_MessageBoxW)
		(hWnd, pszText, pszCaption, uType);

	// 拦截的同时向窗口发送消息
	SendLastMsgBoxInfo(TRUE, (PVOID)pszCaption, (PVOID)pszText, nResult);

	// 返回
	return(nResult);
}


///////////////////////////////////////////////////////////////////////////////


// MessageBoxA 替代函数
int WINAPI Hook_MessageBoxA(HWND hWnd, PCSTR pszText, PCSTR pszCaption,
	UINT uType) {

	int nResult = ((PFNMESSAGEBOXA)(PROC)g_MessageBoxA)
		(hWnd, pszText, pszCaption, uType);

	// 注意第一个参数的作用
	SendLastMsgBoxInfo(FALSE, (PVOID)pszCaption, (PVOID)pszText, nResult);

	return(nResult);
}


///////////////////////////////////////////////////////////////////////////////


// 初始化加hook
CAPIHook g_MessageBoxA("User32.dll", "MessageBoxA",
(PROC)Hook_MessageBoxA);

CAPIHook g_MessageBoxW("User32.dll", "MessageBoxW",
(PROC)Hook_MessageBoxW);

HHOOK g_hhook = NULL;


///////////////////////////////////////////////////////////////////////////////


// 钩子处理函数 由于是为了注入 所以什么都不用做
static LRESULT WINAPI GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
	return(CallNextHookEx(g_hhook, code, wParam, lParam));
}


///////////////////////////////////////////////////////////////////////////////

// 返回指定内存所在的HMODULE
static HMODULE ModuleFromAddress(PVOID pv) {

	MEMORY_BASIC_INFORMATION mbi;
	return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}


///////////////////////////////////////////////////////////////////////////////

// 一个导出函数 被用来注入
// 这里直接将SetWindowsHookEx函数封装到Dll中 并且开放了TID参数 妙不可言
BOOL WINAPI LastMsgBoxInfo_HookAllApps(BOOL bInstall, DWORD dwThreadId) {

	BOOL bOk;

	if (bInstall) {

		chASSERT(g_hhook == NULL); // 防止重复注入

		// 安装钩子
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
			ModuleFromAddress(LastMsgBoxInfo_HookAllApps), dwThreadId);

		bOk = (g_hhook != NULL);
	}
	else {

		chASSERT(g_hhook != NULL); // 没有被注入就不能卸载钩子
		bOk = UnhookWindowsHookEx(g_hhook);
		g_hhook = NULL;
	}

	return(bOk);
}


//////////////////////////////////////////////////////////////////


