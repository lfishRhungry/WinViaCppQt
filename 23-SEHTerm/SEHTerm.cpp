/*һ��Ϊ��չʾSEH��ȫ��չ���ĸ���ĳ���
  ע��ֻ���ڸ���vista��ϵͳ�汾������*/
#include "../CommonFiles/CmnHdr.h"
#include <windows.h>
#include <tchar.h>


///////////////////////////////////////////////////////////////////////////////


// ֻ����vista���ϰ汾���ܽ���SEHȫ��չ�� ����Ҫ����
BOOL IsGreaterThanWindowsVista() {

	// ׼��ָ��OSVERSIONGINFOEX�ṹ
	OSVERSIONINFOEX osver = { 0 };
	osver.dwOSVersionInfoSize = sizeof(osver);
	osver.dwMajorVersion = 6;
	osver.dwMinorVersion = 0;
	osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

	// ���Է���
	DWORDLONG dwlConditionMask = 0;	// ��������ȳ�ʼ��Ϊ0
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_GREATER_EQUAL);

	// ���а汾����
	if (VerifyVersionInfo(&osver, VER_MAJORVERSION | VER_MINORVERSION |
		VER_PLATFORMID, dwlConditionMask)) {
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}


void TriggerException() {

	__try {
		int n = MessageBox(NULL, TEXT("Perform invalid memory access?"),
			TEXT("SEHTerm: In try block"), MB_YESNO);

		if (n == IDYES) {
			*(PBYTE)NULL = 5;  // ����Υ�����
		}
	}
	__finally {
		PCTSTR psz = AbnormalTermination()
			? TEXT("Abnormal termination") : TEXT("Normal termination");
		MessageBox(NULL, psz, TEXT("SEHTerm: In finally block"), MB_OK);
	}

	MessageBox(NULL, TEXT("Normal function termination"),
		TEXT("SEHTerm: After finally block"), MB_OK);
}


int WINAPI _tWinMain(HINSTANCE, HINSTANCE, PTSTR, int) {

	// ��windows vista֮����������һ���쳣����������EXCEPTION_EXECUTE_HANDLER
	// �ͻᴥ��ȫ��չ�������һ�����ܱ�������쳣�����ˣ����̾ͻ�򵥵���ֹ��finally
	// ��Ҳ���ᱻִ��

	// ��ϵͳ�汾����Vista���û�ͬ�� ����__try���� ������ȫ��չ��
	if (IsGreaterThanWindowsVista()) {

		DWORD n = MessageBox(NULL, TEXT("Protect with try/except?"),
			TEXT("SEHTerm: workflow"), MB_YESNO);

		if (n == IDYES) {
			__try {
				TriggerException();
			}
			// �����Ҫȫ��չ�� �����Ҫ��������EXCEPTION_EXECUTE_HANDLER

			// ֻ���������������finally���ִ�У�
			// 1 ��try��������finally
			// 2 �ֲ�չ������try����ǰ�����˳���return goto longjump continue break�ȣ�
			// 3 �����ȫ��չ�� �����������쳣 ���ұ�����ϲ���մ��쳣���͵� �ͻ�ӱ���
			//   ��finallyһֱִ�е���һ�� ֻ����except����ӹ��˺���������EXCEPTION_
			//   EXECUTE_HANDLER �ſ�������ȫ��չ��
			__except (EXCEPTION_EXECUTE_HANDLER) {

				MessageBox(NULL, TEXT("Abnormal process termination"),
					TEXT("Process entry point try/except handler"), MB_OK);

				return(-1);
			}
		}
		else {
			TriggerException();
		}
	}
	else {
		TriggerException();
	}

	MessageBox(NULL, TEXT("Normal process termination"),
		TEXT("SEHTerm: before leaving the main thread"), MB_OK);

	return(0);
}


//////////////////////////////// End of File //////////////////////////////////
