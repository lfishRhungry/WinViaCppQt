/*
演示了windows线程池如何设置定时线程
*/

#include "../CommonFiles/CmnHdr.h"
#include <tchar.h>
#include <StrSafe.h>


//////////////////////////////////////////////////////////////////////////////


// messagebox标题
TCHAR g_szCaption[100];


// 展示messagebox的秒数
int g_nSecLeft = 0;


// This is STATIC window control ID for a message box
#define ID_MSGBOX_STATIC_TEXT    0x0000ffff


//////////////////////////////////////////////////////////////////////////////


// 定义一个根据时间触发的线程函数
VOID CALLBACK MsgBoxTimeoutCallback(
	PTP_CALLBACK_INSTANCE   pInstance,
	PVOID                   pvContext,
	PTP_TIMER               pTimer) {
	// 由于线程的竞争关系，有一丢丢的可能性，代码执行到这里时 messagebox还没有被创建
	HWND hwnd = FindWindow(NULL, g_szCaption);

	if (hwnd != NULL) {
		if (g_nSecLeft == 1) {
			// 时间到 强制退出
			EndDialog(hwnd, IDOK);
			return;
		}

		// 还没到时间 更新时间
		TCHAR szMsg[100];
		StringCchPrintf(szMsg, _countof(szMsg),
			TEXT("You have %d seconds to respond"), --g_nSecLeft);
		SetDlgItemText(hwnd, ID_MSGBOX_STATIC_TEXT, szMsg);
	}
	else {
		// 还拿不到hwnd 说明窗口还没被创建 等待下一次回调
	}
}

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, PTSTR, int) {

	_tcscpy_s(g_szCaption, _countof(g_szCaption), TEXT("Timed Message Box"));

	// 设置起始时间
	g_nSecLeft = 10;

	// 创建线程池计时对象
	PTP_TIMER lpTimer =
		CreateThreadpoolTimer(MsgBoxTimeoutCallback, NULL, NULL);

	if (lpTimer == NULL) {
		TCHAR szMsg[MAX_PATH];
		StringCchPrintf(szMsg, _countof(szMsg),
			TEXT("Impossible to create the timer: %u"), GetLastError());
		MessageBox(NULL, szMsg, TEXT("Error"), MB_OK | MB_ICONERROR);

		return(-1);
	}

	// 一秒钟之后计时器开始(负数表示相对时间)
	ULARGE_INTEGER ulRelativeStartTime;
	ulRelativeStartTime.QuadPart = (LONGLONG)-(10000000);  // start in 1 second
	FILETIME ftRelativeStartTime;
	ftRelativeStartTime.dwHighDateTime = ulRelativeStartTime.HighPart;
	ftRelativeStartTime.dwLowDateTime = ulRelativeStartTime.LowPart;
	// 设置线程池计时开始
	SetThreadpoolTimer(
		lpTimer,
		&ftRelativeStartTime,
		1000, // 一秒触发一次
		0
	);

	// Display the message box
	MessageBox(NULL, TEXT("You have 10 seconds to respond"),
		g_szCaption, MB_OK);

	// 清理计时器
	CloseThreadpoolTimer(lpTimer);

	// 无论如何 当前一个messagebox关闭之后，我们都可以通过剩余时间的值
	// 来判断是用户关闭了对话框还是超时关闭
	MessageBox(
		NULL, (g_nSecLeft == 1) ? TEXT("Timeout") : TEXT("User responded"),
		TEXT("Result"), MB_OK);

	return(0);
}


//////////////////////////////// End of File /////////////////////////////////
