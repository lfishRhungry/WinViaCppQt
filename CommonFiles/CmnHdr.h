/******************************************************************************
摘录自Jeffrey Richter的在《Windows核心编程》中的公共头文件 定义了一些需要使用的
宏、模板以及函数等
******************************************************************************/


#pragma once   // 本头文件只能包含并编译一次

//////////////////////// Windows构建选项 确保能使用vista以上的函数 /////////////////////////


// = 0x0600 for VISTA level from sdkddkver.h
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN 
#define WINVER       _WIN32_WINNT_LONGHORN 


//////////////////////////// Unicode生成选项 /////////////////////////////


// 通知编译器始终使用unicode
#ifndef UNICODE
	#define UNICODE
#endif

// 同时也要使用Unicode版本的c运行库
#ifdef UNICODE
   #ifndef _UNICODE
      #define _UNICODE
   #endif
#endif


///////////////////////// Include Windows Definitions /////////////////////////

#pragma warning(push, 3) // 在头文件中就不要使用高等级代码警告
#include <Windows.h>
#pragma warning(pop) 
#pragma warning(push, 4)
#include <CommCtrl.h>
#include <process.h>       // 为了调用 _beginthreadex
#include <stdlib.h>
//#include <QtDebug> // Qt调试专用

///////////// Verify that the proper header files are being used //////////////


#ifndef FILE_SKIP_COMPLETION_PORT_ON_SUCCESS
#pragma message("You are not using the latest Platform SDK header/library ")
#pragma message("files. This may prevent the project from building correctly.")
#endif

////////////// 在等级4的时候取消一些无关紧要的警告 ///////////////


/* nonstandard extension 'single line comment' was used */
#pragma warning(disable:4001)

// unreferenced formal parameter
#pragma warning(disable:4100)

// Note: Creating precompiled header 
#pragma warning(disable:4699)

// function not inlined
#pragma warning(disable:4710)

// unreferenced inline function has been removed
#pragma warning(disable:4514)

// assignment operator could not be generated
#pragma warning(disable:4512)

// conversion from 'LONGLONG' to 'ULONGLONG', signed/unsigned mismatch
#pragma warning(disable:4245)

// 'type cast' : conversion from 'LONG' to 'HINSTANCE' of greater size
#pragma warning(disable:4312)

// 'argument' : conversion from 'LPARAM' to 'LONG', possible loss of data
#pragma warning(disable:4244)

// 'wsprintf': name was marked as #pragma deprecated
#pragma warning(disable:4995)

// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable:4146)

// 'argument' : conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable:4267)

// nonstandard extension used : nameless struct/union
#pragma warning(disable:4201)

///////////////////////// 方便编译时在代码中定位我们预设位置的宏 /////////////////////////


/* 
When the compiler sees a line like this:
   #pragma chMSG(Fix this later)

it outputs a line like this:

  c:\CD\CmnHdr.h(82):Fix this later

You can easily jump directly to this line and examine the surrounding code.
*/

#define chSTR2(x) #x
#define chSTR(x)  chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)


////////////////////////////// chINRANGE Macro 可以确定是否在两个值之间 ////////////////////////////////


// This macro returns TRUE if a number is between two others
#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))



///////////////////////////// chSIZEOFSTRING Macro ////////////////////////////

// 计算以零结尾的字符串的字节数

// This macro evaluates to the number of bytes needed by a string.
#define chSIZEOFSTRING(psz)   ((lstrlen(psz) + 1) * sizeof(TCHAR))



/////////////////// chROUNDDOWN & chROUNDUP inline functions //////////////////


// This inline function rounds a value down to the nearest multiple
template <class TV, class TM>
inline TV chROUNDDOWN(TV Value, TM Multiple) {
   return((Value / Multiple) * Multiple);
}


// This inline function rounds a value down to the nearest multiple
template <class TV, class TM>
inline TV chROUNDUP(TV Value, TM Multiple) {
   return(chROUNDDOWN(Value, Multiple) + 
      (((Value % Multiple) > 0) ? Multiple : 0));
}



///////////////////////////// chBEGINTHREADEX Macro ///////////////////////////

// 方便使用_beginthreadex函数

// This macro function calls the C runtime's _beginthreadex function. 
// The C runtime library doesn't want to have any reliance on Windows' data 
// types such as HANDLE. This means that a Windows programmer needs to cast
// values when using _beginthreadex. Since this is terribly inconvenient, 
// I created this macro to perform the casting.
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
   pvParam, dwCreateFlags, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                          \
         (void *)        (psa),                         \
         (unsigned)      (cbStackSize),                 \
         (PTHREAD_START) (pfnStartAddr),                \
         (void *)        (pvParam),                     \
         (unsigned)      (dwCreateFlags),               \
         (unsigned *)    (pdwThreadId)))

////////////////// DebugBreak Improvement for x86 platforms ///////////////////

// x86平台下插入软中断
#ifdef _X86_
   #define DebugBreak()    _asm { int 3 }
#endif



/////////////////////////// Software Exception Macro //////////////////////////


// Useful macro for creating your own software exception codes
#define MAKESOFTWAREEXCEPTION(Severity, Facility, Exception) \
   ((DWORD) ( \
   /* Severity code    */  (Severity       ) |     \
   /* MS(0) or Cust(1) */  (1         << 29) |     \
   /* Reserved(0)      */  (0         << 28) |     \
   /* Facility code    */  (Facility  << 16) |     \
   /* Exception code   */  (Exception <<  0)))


/////////////////////////// Quick MessageBox Macro ////////////////////////////

// 方便使用messageBox

inline void chMB(PCSTR szMsg) {
   char szTitle[MAX_PATH];
   GetModuleFileNameA(NULL, szTitle, _countof(szTitle));
   MessageBoxA(GetActiveWindow(), szMsg, szTitle, MB_OK);
}


//////////////////////////// Assert/Verify Macros /////////////////////////////


inline void chFAIL(PSTR szMsg) {
   chMB(szMsg);
   DebugBreak();
}


// Put up an assertion failure message box.
inline void chASSERTFAIL(LPCSTR file, int line, PCSTR expr) {
   char sz[2*MAX_PATH];
   wsprintfA(sz, "File %s, line %d : %s", file, line, expr);
   chFAIL(sz);
}


// Put up a message box if an assertion fails in a debug build.
#ifdef _DEBUG
   #define chASSERT(x) if (!(x)) chASSERTFAIL(__FILE__, __LINE__, #x)
#else
   #define chASSERT(x)
#endif


// Assert in debug builds, but don't remove the code in retail builds.
#ifdef _DEBUG
   #define chVERIFY(x) chASSERT(x)
#else
   #define chVERIFY(x) (x)
#endif


/////////////////////////// chHANDLE_DLGMSG Macro /////////////////////////////


// The normal HANDLE_MSG macro in WindowsX.h does not work properly for dialog
// boxes because DlgProc returns a BOOL instead of an LRESULT (like
// WndProcs). This chHANDLE_DLGMSG macro corrects the problem:
#define chHANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

/////////////////////////// Common Linker Settings ////////////////////////////


#pragma comment(linker, "/nodefaultlib:oldnames.lib")


///////////////////////////////// End of File /////////////////////////////////
