/******************************************************************************
Jeffrey Richter对virtualQuery函数的扩展版本
******************************************************************************/
#include "../../CommonFiles/CmnHdr.h"
#include <windowsx.h>

// 自定义一个VMQUERY结构 且会用VMQuery函数填充
typedef struct {
	// 区域信息
	PVOID  pvRgnBaseAddress; // 该区域的起始地址
	DWORD  dwRgnProtection;  // 最开始预定该区域时指定的保护属性
	SIZE_T RgnSize;          // 所预定区域大小（字节单位）
	DWORD  dwRgnStorage;     // 存储器类型（free image mapped private）
	DWORD  dwRgnBlocks;      // 区域中块的数量
	DWORD  dwRgnGuardBlks;   // 该区域中PAGE_GUARD属性块数量（位线程栈预定）
	BOOL   bRgnIsAStack;     // 该区域是否包含线程栈（是一个猜测值）

	// 块信息
	PVOID  pvBlkBaseAddress; // 块起始地址
	DWORD  dwBlkProtection;  // 块保护属性
	SIZE_T BlkSize;          // 块大小（字节单位）
	DWORD  dwBlkStorage;     // 块的存储器类型
} VMQUERY, * PVMQUERY;


///////////////////////////////////////////////////////////////////////////////


BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ);


//////////////////////////////// End of File //////////////////////////////////
