/*用于执行IAT hook的类*/
#pragma once

 class CAPIHook
{
public:
	// 在所有模块中hook一个指定函数
	CAPIHook(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook);

	// 在所有模块中unhook一个指定函数
	~CAPIHook();

	// 这里重写了PROC()操作符 返回被hook函数的原地址
	operator PROC() { return(m_pfnOrig); }

	// 是否对我们自己注入的模块执行hook操作
	// 这里使用静态是因为要在静态成员函数ReplaceIATEntryInAllMods中使用
	static BOOL ExcludeAPIHookMod;


public:
	// 调用真·GetProcAddress
	static FARPROC WINAPI GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName);

private:
	static PVOID sm_pvMaxAppAddr; // 最大私有内存空间地址
	static CAPIHook* sm_pHead;    // CAPIHook对象链表头
	CAPIHook* m_pNext;            // 在CAPIHook对象链表中指向下一个对象

	PCSTR m_pszCalleeModName;     // 包含指定函数的模块名 (ANSI)
	PCSTR m_pszFuncName;          // 函数名 (ANSI)
	PROC  m_pfnOrig;              // 函数原地址
	PROC  m_pfnHook;              // hook函数的地址

private:
	// 在所有模块中的IAT替换指定符号地址
	static void WINAPI ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook);

	// 在一个模块中的IAT替换指定符号地址
	static void WINAPI ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook, HMODULE hmodCaller);

	// 在一个模块中的EAT替换指定符号地址
	static void ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName, PROC pfnNew);

private:
	// 用来处理在我们hook操作之后才新加载的DLL
	static void    WINAPI FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags);

	// 用来拦截新加载DLL的情况
	static HMODULE WINAPI LoadLibraryA(PCSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryW(PCWSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryExA(PCSTR pszModulePath,
		HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI LoadLibraryExW(PCWSTR pszModulePath,
		HANDLE hFile, DWORD dwFlags);

	// 在请求被hook函数时用来返回替代函数的地址
	static FARPROC WINAPI GetProcAddress(HMODULE hmod, PCSTR pszProcName);

private:
	// 为了全面hook指定函数 还需要hook下列函数
	static CAPIHook sm_LoadLibraryA;
	static CAPIHook sm_LoadLibraryW;
	static CAPIHook sm_LoadLibraryExA;
	static CAPIHook sm_LoadLibraryExW;
	static CAPIHook sm_GetProcAddress;
};


