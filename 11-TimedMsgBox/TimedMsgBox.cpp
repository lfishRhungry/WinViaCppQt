/*
��ʾ��windows�̳߳�������ö�ʱ�߳�
*/

#include "../CommonFiles/CmnHdr.h"
#include <tchar.h>
#include <StrSafe.h>


//////////////////////////////////////////////////////////////////////////////


// messagebox����
TCHAR g_szCaption[100];


// չʾmessagebox������
int g_nSecLeft = 0;


// This is STATIC window control ID for a message box
#define ID_MSGBOX_STATIC_TEXT    0x0000ffff


//////////////////////////////////////////////////////////////////////////////


// ����һ������ʱ�䴥�����̺߳���
VOID CALLBACK MsgBoxTimeoutCallback(
	PTP_CALLBACK_INSTANCE   pInstance,
	PVOID                   pvContext,
	PTP_TIMER               pTimer) {
	// �����̵߳ľ�����ϵ����һ�����Ŀ����ԣ�����ִ�е�����ʱ messagebox��û�б�����
	HWND hwnd = FindWindow(NULL, g_szCaption);

	if (hwnd != NULL) {
		if (g_nSecLeft == 1) {
			// ʱ�䵽 ǿ���˳�
			EndDialog(hwnd, IDOK);
			return;
		}

		// ��û��ʱ�� ����ʱ��
		TCHAR szMsg[100];
		StringCchPrintf(szMsg, _countof(szMsg),
			TEXT("You have %d seconds to respond"), --g_nSecLeft);
		SetDlgItemText(hwnd, ID_MSGBOX_STATIC_TEXT, szMsg);
	}
	else {
		// ���ò���hwnd ˵�����ڻ�û������ �ȴ���һ�λص�
	}
}

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, PTSTR, int) {

	_tcscpy_s(g_szCaption, _countof(g_szCaption), TEXT("Timed Message Box"));

	// ������ʼʱ��
	g_nSecLeft = 10;

	// �����̳߳ؼ�ʱ����
	PTP_TIMER lpTimer =
		CreateThreadpoolTimer(MsgBoxTimeoutCallback, NULL, NULL);

	if (lpTimer == NULL) {
		TCHAR szMsg[MAX_PATH];
		StringCchPrintf(szMsg, _countof(szMsg),
			TEXT("Impossible to create the timer: %u"), GetLastError());
		MessageBox(NULL, szMsg, TEXT("Error"), MB_OK | MB_ICONERROR);

		return(-1);
	}

	// һ����֮���ʱ����ʼ(������ʾ���ʱ��)
	ULARGE_INTEGER ulRelativeStartTime;
	ulRelativeStartTime.QuadPart = (LONGLONG)-(10000000);  // start in 1 second
	FILETIME ftRelativeStartTime;
	ftRelativeStartTime.dwHighDateTime = ulRelativeStartTime.HighPart;
	ftRelativeStartTime.dwLowDateTime = ulRelativeStartTime.LowPart;
	// �����̳߳ؼ�ʱ��ʼ
	SetThreadpoolTimer(
		lpTimer,
		&ftRelativeStartTime,
		1000, // һ�봥��һ��
		0
	);

	// Display the message box
	MessageBox(NULL, TEXT("You have 10 seconds to respond"),
		g_szCaption, MB_OK);

	// �����ʱ��
	CloseThreadpoolTimer(lpTimer);

	// ������� ��ǰһ��messagebox�ر�֮�����Ƕ�����ͨ��ʣ��ʱ���ֵ
	// ���ж����û��ر��˶Ի����ǳ�ʱ�ر�
	MessageBox(
		NULL, (g_nSecLeft == 1) ? TEXT("Timeout") : TEXT("User responded"),
		TEXT("Result"), MB_OK);

	return(0);
}


//////////////////////////////// End of File /////////////////////////////////
