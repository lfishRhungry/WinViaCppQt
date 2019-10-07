#include "pch.h"
#include "APIHook.h"
#include "../CommonFiles/CmnHdr.h"
#include "../CommonFiles/Toolhelp.h"
#include <strsafe.h>
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")


/////////////////////////////////////////////////////////////////////////////////


// 初始化CAPIHook对象链表头
// 别忘了这个是在类定义中初始化静态成员变量的方法
// 这里只会初始化这么一次
CAPIHook* CAPIHook::sm_pHead = NULL;

// 默认我们的模块是不被hook的
BOOL CAPIHook::ExcludeAPIHookMod = TRUE;


///////////////////////////////////////////////////////////////////////////////


CAPIHook::CAPIHook(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook) {

	/*
	 注意 我们在执行hook之前，目标进程必须已经导入了我们要hook的函数的所属模块
		  否则我们拿不到被hook函数原地址 也不能修改相应的导入表内容
	 一种解决方案 用被拦截的LoadLibrary系列函数来检查所载入模块的导出表中是否有
		  我们要hook的函数 如果有 那么我们就拿到了被hook函数原地址 回头将之前已
		  经载入的所有模块导入表进行更新 同时，对被hook函数所在模块的导出表进行
		  更改 这么一来在之后所有新载入的模块都会调用我们的替换函数了
	*/

	// 初始化CAPIHook链表
	// 由于sm_mpHead是静态成员变量
	// 所以这么做相当于动态将本CAPIHook对象添加都链表头部
	m_pNext = sm_pHead;
	sm_pHead = this;

	// 保存本CAPIHook对象要hook的信息
	// 这起始是防止循环hook的一个好方式
	// 让每一个CAPIHook对象始终知道自己的任务
	m_pszCalleeModName = pszCalleeModName;
	m_pszFuncName = pszFuncName;
	m_pfnHook = pfnHook;
	m_pfnOrig =
		GetProcAddressRaw(GetModuleHandleA(pszCalleeModName), m_pszFuncName);

	// 如果函数不存在（可能是未被导入）拜拜了您嘞~
	if (m_pfnOrig == NULL)
	{
#ifdef _DEBUG
		// 这里苟延残喘输出调试信息
		wchar_t szPathname[MAX_PATH];
		GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
		wchar_t sz[1024];
		StringCchPrintfW(sz, _countof(sz),
			TEXT("[%4u - %s] impossible to find %S\r\n"),
			GetCurrentProcessId(), szPathname, pszFuncName);
		OutputDebugString(sz);
#endif
		return;
	}

#ifdef _DEBUG
	// This section was used for debugging sessions when Explorer died as 
	// a folder content was requested
	// 这里是作者用来辅助调试的
	// 中间涉及到的情景是：当注入到Explorer进程，并且Explorer要执行打开文件夹操作
	// 
	//static BOOL s_bFirstTime = TRUE;
	//if (s_bFirstTime)
	//{
	//   s_bFirstTime = FALSE;

	//   wchar_t szPathname[MAX_PATH];
	//   GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
	//   wchar_t* pszExeFile = wcsrchr(szPathname, L'\\') + 1;
	//   OutputDebugStringW(L"Injected in ");
	//   OutputDebugStringW(pszExeFile);
	//   if (_wcsicmp(pszExeFile, L"Explorer.EXE") == 0)
	//   {
	//      DebugBreak();
	//   }
	//   OutputDebugStringW(L"\n   --> ");
	//   StringCchPrintfW(szPathname, _countof(szPathname), L"%S", pszFuncName);
	//   OutputDebugStringW(szPathname);
	//   OutputDebugStringW(L"\n");
	//}
#endif

   // 开始hook吧~
	ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnOrig, m_pfnHook);
}


///////////////////////////////////////////////////////////////////////////////


CAPIHook::~CAPIHook() {

	// 调转地址 达到unhook的效果
	ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnHook, m_pfnOrig);

	// 将本CAPIHook对象从链表中移除
	CAPIHook* p = sm_pHead;
	if (p == this) {     // Removing the head node
		sm_pHead = p->m_pNext;
	}
	else {

		BOOL bFound = FALSE;

		for (; !bFound && (p->m_pNext != NULL); p = p->m_pNext) {
			if (p->m_pNext == this) {
				p->m_pNext = p->m_pNext->m_pNext;
				bFound = TRUE;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


// 这个函数是用来在调用hook程序的进程中来调用真正的GetProcAddress函数
// 只是用来和替代版本做名称区分而方便使用
FARPROC CAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName) {

	return(::GetProcAddress(hmod, pszProcName));
}


///////////////////////////////////////////////////////////////////////////////


// 返回指定内存地址所在模块HMODULE （不是类成员函数）
static HMODULE ModuleFromAddress(PVOID pv) {

	MEMORY_BASIC_INFORMATION mbi;
	return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew) {

	HMODULE hmodThisMod = ExcludeAPIHookMod
		? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;

	// 获取目标进程模块列表快照
	CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());

	MODULEENTRY32 me = { sizeof(me) };
	for (BOOL bOk = th.ModuleFirst(&me); bOk; bOk = th.ModuleNext(&me)) {

		// 避免hook我们自己的模块(具体看ExcludeAPIHookMod变量的设置)
		if (me.hModule != hmodThisMod) {

			// Hook this function in this module
			ReplaceIATEntryInOneMod(
				pszCalleeModName, pfnCurrent, pfnNew, me.hModule);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


// 用来处理当模块被卸载时发生的不可预知异常的SEH过滤函数
LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS pep) {

	// 无论如何都返回EXCEPTION_EXECUTE_HANDLER
	// 表示能够处理
	LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;


	return(lDisposition);
}


// 核心技术点之一
void CAPIHook::ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller) {

	// 获取模块导入段地址
	ULONG ulSize;

	//对于特殊的目标程序比如说Explorer来说：它在扫描文件夹的时候，是会快速地载入和卸载dll，
	//可能就会导致我们在这里传入给ImageDirectoryEntryToData函数的句柄失效。
	//也有可能是我们使用Toolhelp得到模块快照之后，该模块又被卸载了，导致句柄失效
	//总而言之，很有必要使用SEH来保护这段代码
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	__try {
		// 该函数定位模块的导入段
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
			hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation())) {
		// 这里什么也不需要处理，只不过此时pImportDesc是NULL
	}

	if (pImportDesc == NULL)
		return;  // 此模块无导入段


	// 遍历IMAGE_IMPORT_DESCRIPTOR结构中的名称来找到关联指定模块的IAT
	for (; pImportDesc->Name; pImportDesc++) {
		PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name); // RVA转换取得名称
		// 找到了~
		if (lstrcmpiA(pszModName, pszCalleeModName) == 0) {

			// 定位到指定导入模块的第一个IMAGE_THUNK_DATA结构
			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
				((PBYTE)hmodCaller + pImportDesc->FirstThunk);

			// 遍历各个IMAGE_THUNK_DATA来找到存放被hook函数地址的位置
			// 如果说这个模块已经被hook过了 就找不到了
			for (; pThunk->u1.Function; pThunk++) {

				PROC* ppfn = (PROC*)&pThunk->u1.Function;

				// 比较其中存放的地址是否是我们指定的函数地址
				BOOL bFound = (*ppfn == pfnCurrent);
				if (bFound) {
					if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
						sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
						// 如果写失败就修改权限再写
						DWORD dwOldProtect;
						// 这里使用写时复制是防止对源文件进行修改可能造成的损害
						if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
							&dwOldProtect)) {

							WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
								sizeof(pfnNew), NULL);
							// 记得恢复原来的保护属性
							VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect,
								&dwOldProtect);
						}
					}
					return;  // 修改完成 走嘞~
				}
			}
		}  // 有些编译器导致同一个模块可能有多个导入段 所以如果没有修改到的话 这里还要继续循环寻找
	}
}


///////////////////////////////////////////////////////////////////////////////


// 这里给出了这个函数 但是没有用到 只是提供了处理延时载入情况的可能用到的工具
void CAPIHook::ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName,
	PROC pfnNew) {

	// 获取该模块导出表地址
	ULONG ulSize;

	// 和修改IAT的情况一样 因为获取模块句柄和通过句柄获取得到导出段并不是原子操作
	// 所以要考虑到模块句柄失效的可能
	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
	__try {
		pExportDir = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(
			hmod, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation())) {}

	if (pExportDir == NULL)
		return;  // 没有导出表或已经模块已被卸载

	PDWORD pdwNamesRvas = (PDWORD)((PBYTE)hmod + pExportDir->AddressOfNames);
	PWORD pdwNameOrdinals = (PWORD)
		((PBYTE)hmod + pExportDir->AddressOfNameOrdinals);
	PDWORD pdwFunctionAddresses = (PDWORD)
		((PBYTE)hmod + pExportDir->AddressOfFunctions);

	// 遍历模块函数名数组
	for (DWORD n = 0; n < pExportDir->NumberOfNames; n++) {
		PSTR pszFuncName = (PSTR)((PBYTE)hmod + pdwNamesRvas[n]);

		// 不是指定函数就continue
		if (lstrcmpiA(pszFuncName, pszFunctionName) != 0) continue;

		WORD ordinal = pdwNameOrdinals[n];

		// 获取原函数地址所在地址
		PROC* ppfn = (PROC*)&pdwFunctionAddresses[ordinal];

		// 将替换函数地址转换为RVA
		pfnNew = (PROC)((PBYTE)pfnNew - (PBYTE)hmod);

		// 修改之
		if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
			sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
			DWORD dwOldProtect;
			if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
				&dwOldProtect)) {

				WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
					sizeof(pfnNew), NULL);
				VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect, &dwOldProtect);
			}
		}
		break;  // 修改成果 完美
	}
}


///////////////////////////////////////////////////////////////////////////////
// 将LoadLibrary系列函数和GeetProcAddress函数hook掉 这样一旦调用这几个函数来
// 调用我们要hook的函数 一切尽在掌握中
// 这里都是静态成员变量 避免每次实例化一个CAPIHook对象都会生成这几个对象造成无限调用
// 并且每次注入成功时 都只会初始化这么一份LoadLibrary系列函数和GetProcAddress函数
// 确保针对每一个指定hook的函数都之一配套的一份下列保障hook操作

CAPIHook CAPIHook::sm_LoadLibraryA((PSTR)"Kernel32.dll", (PSTR)"LoadLibraryA",
(PROC)CAPIHook::LoadLibraryA);

CAPIHook CAPIHook::sm_LoadLibraryW("Kernel32.dll", "LoadLibraryW",
(PROC)CAPIHook::LoadLibraryW);

CAPIHook CAPIHook::sm_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA",
(PROC)CAPIHook::LoadLibraryExA);

CAPIHook CAPIHook::sm_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW",
(PROC)CAPIHook::LoadLibraryExW);

CAPIHook CAPIHook::sm_GetProcAddress("Kernel32.dll", "GetProcAddress",
(PROC)CAPIHook::GetProcAddress);


///////////////////////////////////////////////////////////////////////////////


// 这里主要是给替代的LoadLibrary函数和GetProcAddress函数内使用的
void CAPIHook::FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags) {

	// 如果载入新模块 就把这个模块中我们的函数hook掉
	if ((hmod != NULL) &&
		// 避免hook掉我们自己的模块
		(hmod != ModuleFromAddress(FixupNewlyLoadedModule)) &&
		// 这几种载入模式就可以不用改了 因为是不可执行的载入方式
		((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0) &&
		((dwFlags & LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) == 0) &&
		((dwFlags & LOAD_LIBRARY_AS_IMAGE_RESOURCE) == 0)
		) {

		// 遍历我们构造的CAPIHook链表 让每个对象再次对所有模块进行hook
		for (CAPIHook* p = sm_pHead; p != NULL; p = p->m_pNext) {
			if (p->m_pfnOrig != NULL) {
				ReplaceIATEntryInAllMods(p->m_pszCalleeModName,
					p->m_pfnOrig, p->m_pfnHook);
			}
			else {
#ifdef _DEBUG
				// 说实在的应该不可能执行到这里。。。
				wchar_t szPathname[MAX_PATH];
				GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
				wchar_t sz[1024];
				StringCchPrintfW(sz, _countof(sz),
					TEXT("[%4u - %s] impossible to find %S\r\n"),
					GetCurrentProcessId(), szPathname, p->m_pszCalleeModName);
				OutputDebugString(sz);
#endif
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath) {

	// 说实话这里还挺妙的~
	HMODULE hmod = ::LoadLibraryA(pszModulePath);
	FixupNewlyLoadedModule(hmod, 0);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath) {

	HMODULE hmod = ::LoadLibraryW(pszModulePath);
	FixupNewlyLoadedModule(hmod, 0);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath,
	HANDLE hFile, DWORD dwFlags) {

	HMODULE hmod = ::LoadLibraryExA(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hmod, dwFlags);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath,
	HANDLE hFile, DWORD dwFlags) {

	// 原来这个dwFlags在这里用呀
	HMODULE hmod = ::LoadLibraryExW(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hmod, dwFlags);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName) {

	// 拿到真·GetProcAddress函数地址
	FARPROC pfn = GetProcAddressRaw(hmod, pszProcName);

	// 这里要遍历一下我们CAPIHook链表中有没有要对这个函数进行hook
	// 如果有 就顺势返回对应的替代函数 妙啊！！！
	CAPIHook* p = sm_pHead;
	for (; (pfn != NULL) && (p != NULL); p = p->m_pNext) {

		if (pfn == p->m_pfnOrig) {

			pfn = p->m_pfnHook;
			break;
		}
	}

	return(pfn);
}


//////////////////////////////// End of File //////////////////////////////////

