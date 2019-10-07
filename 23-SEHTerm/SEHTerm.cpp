/*一段为了展示SEH中全局展开的概念的程序
  注意只能在高于vista的系统版本中运行*/
#include "../CommonFiles/CmnHdr.h"
#include <windows.h>
#include <tchar.h>


///////////////////////////////////////////////////////////////////////////////


// 只有在vista以上版本才能进行SEH全局展开 所以要测试
BOOL IsGreaterThanWindowsVista() {

	// 准备指向OSVERSIONGINFOEX结构
	OSVERSIONINFOEX osver = { 0 };
	osver.dwOSVersionInfoSize = sizeof(osver);
	osver.dwMajorVersion = 6;
	osver.dwMinorVersion = 0;
	osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

	// 测试方法
	DWORDLONG dwlConditionMask = 0;	// 这里必须先初始化为0
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_GREATER_EQUAL);

	// 进行版本测试
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
			*(PBYTE)NULL = 5;  // 出发违规访问
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

	// 在windows vista之后，如果，如果一个异常过滤器返回EXCEPTION_EXECUTE_HANDLER
	// 就会触发全局展开。如果一个不能被处理的异常发生了，进程就会简单地终止，finally
	// 块也不会被执行

	// 当系统版本高于Vista且用户同意 就用__try包裹 并设置全局展开
	if (IsGreaterThanWindowsVista()) {

		DWORD n = MessageBox(NULL, TEXT("Protect with try/except?"),
			TEXT("SEHTerm: workflow"), MB_YESNO);

		if (n == IDYES) {
			__try {
				TriggerException();
			}
			// 如果需要全局展开 这里就要主动设置EXCEPTION_EXECUTE_HANDLER

			// 只有三种情况会引发finally块的执行：
			// 1 从try正常流到finally
			// 2 局部展开：从try中提前正常退出（return goto longjump continue break等）
			// 3 这里的全局展开 由于引发了异常 并且本层或上层接收此异常类型的 就会从本层
			//   的finally一直执行到那一层 只有在except中添加过滤函数并返回EXCEPTION_
			//   EXECUTE_HANDLER 才可以引发全局展开
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
