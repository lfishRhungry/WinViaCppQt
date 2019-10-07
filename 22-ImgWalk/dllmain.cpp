/*
用来作为测试dll注入的动态链接库文件
注入之后会自动列出目标进程载入的模块信息
本dll无导出函数
*/
// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "../CommonFiles/CmnHdr.h"
#include <tchar.h>
#include <stdlib.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		char szBuf[MAX_PATH * 100] = { 0 };

		PBYTE pb = NULL; // 从0开始遍历进程的虚拟地址空间
		MEMORY_BASIC_INFORMATION mbi;
		while (VirtualQuery(pb, &mbi, sizeof(mbi)) == sizeof(mbi)) {

			int nLen;
			char szModName[MAX_PATH];

			if (mbi.State == MEM_FREE)
				//如果是FREE状态 那么AllocationBase就无意义
				// 这里让AllocationBase等于BaseAddress
				// 防止AllocationBase的值扰乱接下来的判断
				mbi.AllocationBase = mbi.BaseAddress;

			if ((mbi.AllocationBase == hModule) ||
				(mbi.AllocationBase != mbi.BaseAddress) ||
				(mbi.AllocationBase == NULL)) {
				/*以上三种情况不把这个模块名加入到列表：
				1.如果该区域包含了本DLL
				2.如果该块表示所在区域的首块
				3.区域基地址为NULL
				*/
				nLen = 0;
			}
			else {
				// 注意这里使用的是ANSI类型
				nLen = GetModuleFileNameA((HINSTANCE)mbi.AllocationBase,
					szModName, _countof(szModName));
			}

			if (nLen > 0) {
				wsprintfA(strchr(szBuf, 0), "\n%p-%s",
					mbi.AllocationBase, szModName);
			}

			/* 
			 根据我阅读书上及示例代码
			 这里的RegionSize其实是块大小而不是区域大小
			 所以上面才有判断某块是否是区域首块的代码
			 同时 这里根据我的理解 修改了一下
			 使用RegionSize加上对应的BaseAddress
			 这样合理一些
			*/ 
			pb = (PBYTE)(mbi.BaseAddress) + mbi.RegionSize;
		}

		// 正常来说不要直接在DLL中使用messageBox（可能造成堵塞或死锁）
		// 不过这里为了简单起见就直接使用了
		chMB(&szBuf[1]);
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

