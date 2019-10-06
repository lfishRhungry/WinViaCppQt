// Module:  Toolhelp.h
// 来自Jeffrey Richter & Christophe Nasarre的实用的C++类
// 对toolhelp系列函数的一些封装
// 主要功能有：
// 启动或禁用当前进程的指定权限
// 读取指定进程指定内存地址的指定大小数据

// 遍历系统当前进程快照
// 通过进程快照和pid寻找指定进程的PROCESSENTRY32结构信息

// 遍历系统当前模块快照
// 通过模块基址（module） 寻找指定模块的MODULEENTRY32结构信息
// 通过模块名称 寻找指定模块的MODULEENTRY32结构信息

// 遍历系统当前线程快照

// 通过当前系统堆快照 计算有多少个堆存在
// 通过实时遍历指定pid进程的虚拟地址空间 计算有多少堆块存在
// 遍历系统当前堆快照
// 实时遍历指定pid的进程虚拟地址空间中的堆块

// 通过遍历当前系统堆快照以及实时遍历每个堆中的堆块信息 判断指定地址是否是在堆当中


// 作为头文件来使用 只允许被编译一次
#pragma once


///////////////////////////////////////////////////////////////////////////////


#include "CmnHdr.h"
#include <tlhelp32.h>
#include <tchar.h>


///////////////////////////////////////////////////////////////////////////////

// 注意：C++类中函数声明后面跟上const关键字表示
// 函数内逻辑不允许改变所属对象的成员变量的值
// 这样的函数成为常成员函数


class CToolhelp {
private:
   HANDLE m_hSnapshot;

public:
   CToolhelp(DWORD dwFlags = 0, DWORD dwProcessID = 0);
   ~CToolhelp();
   // 拍摄快照
   BOOL CreateSnapshot(DWORD dwFlags, DWORD dwProcessID = 0);
   // 进程快照相关
   BOOL ProcessFirst(PPROCESSENTRY32 ppe) const;
   BOOL ProcessNext(PPROCESSENTRY32 ppe) const;
   BOOL ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const;
   // 模块快照相关
   BOOL ModuleFirst(PMODULEENTRY32 pme) const;
   BOOL ModuleNext(PMODULEENTRY32 pme) const;
   BOOL ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const;
   BOOL ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const;
   // 线程快照相关
   BOOL ThreadFirst(PTHREADENTRY32 pte) const;
   BOOL ThreadNext(PTHREADENTRY32 pte) const;
   // 堆遍历
   BOOL HeapListFirst(PHEAPLIST32 phl) const;
   BOOL HeapListNext(PHEAPLIST32 phl) const;
   int  HowManyHeaps() const;

   // 注意：下面用于堆块遍历的函数并不是使用快照技术实现的
   // 每次都是是直接遍历正在运行的进程的虚拟地址空间中的堆块
   // 所以 如果出现一种特殊情况 就是当我们正在遍历堆块的时候
   // 进程本身也在改变堆块 这就有可能导致我们的遍历成为死循环
   BOOL HeapFirst(PHEAPENTRY32 phe, DWORD dwProcessID, 
      UINT_PTR dwHeapID) const;
   BOOL HeapNext(PHEAPENTRY32 phe) const;
   int  HowManyBlocksInHeap(DWORD dwProcessID, DWORD dwHeapId) const;
   BOOL IsAHeap(HANDLE hProcess, PVOID pvBlock, PDWORD pdwFlags) const;

public:
   // 启动或禁用进程的指定权限
   static BOOL EnablePrivilege(PCTSTR szPrivilege, BOOL fEnable = TRUE);
   // 读取指定进程指定内存地址的指定大小数据
   static BOOL ReadProcessMemory(DWORD dwProcessID, LPCVOID pvBaseAddress, 
      PVOID pvBuffer, SIZE_T cbRead, SIZE_T* pNumberOfBytesRead = NULL);
};

///////////////////////////////////////////////////////////////////////////////
/*由于类中的函数在使用时往往需要被频繁调用
为了减少占空间的消耗 均采用内联形式*/
///////////////////////////////////////////////////////////////////////////////


inline CToolhelp::CToolhelp(DWORD dwFlags, DWORD dwProcessID) {

   m_hSnapshot = INVALID_HANDLE_VALUE; // 先将快照句柄设为非法值
   CreateSnapshot(dwFlags, dwProcessID);
}


///////////////////////////////////////////////////////////////////////////////


inline CToolhelp::~CToolhelp() {

   if (m_hSnapshot != INVALID_HANDLE_VALUE)
      CloseHandle(m_hSnapshot); // 销毁句柄
}


///////////////////////////////////////////////////////////////////////////////


inline BOOL CToolhelp::CreateSnapshot(DWORD dwFlags, DWORD dwProcessID) {

   if (m_hSnapshot != INVALID_HANDLE_VALUE) // 确保不要保留任何内核对象句柄
      CloseHandle(m_hSnapshot);

   if (dwFlags == 0) {
      m_hSnapshot = INVALID_HANDLE_VALUE; // 没有传入合法flag值
   } else {
      m_hSnapshot = CreateToolhelp32Snapshot(dwFlags, dwProcessID); // 根据要求拍摄快照
   }
   return(m_hSnapshot != INVALID_HANDLE_VALUE);
}


///////////////////////////////////////////////////////////////////////////////

// 启动或禁用进程的指定权限
inline BOOL CToolhelp::EnablePrivilege(PCTSTR szPrivilege, BOOL fEnable) {

   // 通过提升调试权限 使程序可以查看服务程序的信息
   BOOL fOk = FALSE;    // 假设权限提升失败
   HANDLE hToken;

   // 打开所属进程的令牌 指定我们的打开权限为 可以调整指定令牌权限 的权限
   if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {

      // 创建一个TOKEN_PRIVILEGES结构
	  // 设置好要修改的信息
      TOKEN_PRIVILEGES tp;
      tp.PrivilegeCount = 1;
	  // 拿到需要调整的权限的LUID
      LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
      tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0; // 区分指定权限的开启和禁用
	  // 传入结构 修改令牌权限
      AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
      fOk = (GetLastError() == ERROR_SUCCESS);

      // 关闭句柄
      CloseHandle(hToken);
   }
   return(fOk);
}


///////////////////////////////////////////////////////////////////////////////

// 读取指定进程指定内存地址的指定大小数据
inline BOOL CToolhelp::ReadProcessMemory(DWORD dwProcessID, 
   LPCVOID pvBaseAddress, PVOID pvBuffer, SIZE_T cbRead, 
   SIZE_T* pNumberOfBytesRead) {

   return(Toolhelp32ReadProcessMemory(dwProcessID, pvBaseAddress, pvBuffer, 
      cbRead, pNumberOfBytesRead));
}


///////////////////////////////////////////////////////////////////////////////


inline BOOL CToolhelp::ProcessFirst(PPROCESSENTRY32 ppe) const {

   BOOL fOk = Process32First(m_hSnapshot, ppe);
   if (fOk && (ppe->th32ProcessID == 0))
      fOk = ProcessNext(ppe); // 跳过pid为0的系统空闲进程（system idle process）
   return(fOk);
}


inline BOOL CToolhelp::ProcessNext(PPROCESSENTRY32 ppe) const {

   BOOL fOk = Process32Next(m_hSnapshot, ppe);
   if (fOk && (ppe->th32ProcessID == 0))
      fOk = ProcessNext(ppe); // 跳过pid为0的系统空闲进程（system idle process）
   return(fOk);
}


inline BOOL CToolhelp::ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) 
   const {

   BOOL fFound = FALSE;
   // 巧用for循环 通过pid寻找指定进程PROCESSENTRY32结构
   for (BOOL fOk = ProcessFirst(ppe); fOk; fOk = ProcessNext(ppe)) {
      fFound = (ppe->th32ProcessID == dwProcessId);
      if (fFound) break;
   }
   return(fFound);
}


///////////////////////////////////////////////////////////////////////////////


inline BOOL CToolhelp::ModuleFirst(PMODULEENTRY32 pme) const {

   return(Module32First(m_hSnapshot, pme));
}


inline BOOL CToolhelp::ModuleNext(PMODULEENTRY32 pme) const {
   return(Module32Next(m_hSnapshot, pme));
}

// 通过加载基地址（module）寻找指定模块的MODULEENTRY32结构信息
inline BOOL CToolhelp::ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {

   BOOL fFound = FALSE;
   for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
      fFound = (pme->modBaseAddr == pvBaseAddr);
      if (fFound) break;
   }
   return(fFound);
}

// 通过模块名称 寻找指定模块的MODULEENTRY32结构信息
inline BOOL CToolhelp::ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
   BOOL fFound = FALSE;
   for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
	  // 这里同时对比了指定字符串和当前遍历模块的名称以及路径信息
      fFound = (lstrcmpi(pme->szModule,  pszModName) == 0) || 
               (lstrcmpi(pme->szExePath, pszModName) == 0);
      if (fFound) break;
   }
   return(fFound);
}


///////////////////////////////////////////////////////////////////////////////


inline BOOL CToolhelp::ThreadFirst(PTHREADENTRY32 pte) const {

   return(Thread32First(m_hSnapshot, pte));
}


inline BOOL CToolhelp::ThreadNext(PTHREADENTRY32 pte) const {

   return(Thread32Next(m_hSnapshot, pte));
}


///////////////////////////////////////////////////////////////////////////////

// 通过当前系统堆快照 计算有多少个堆存在
inline int CToolhelp::HowManyHeaps() const {

   int nHowManyHeaps = 0;
   HEAPLIST32 hl = { sizeof(hl) };
   for (BOOL fOk = HeapListFirst(&hl); fOk; fOk = HeapListNext(&hl))
      nHowManyHeaps++;
   return(nHowManyHeaps);
}

// 通过实时遍历指定pid进程的虚拟地址空间 计算有多少堆块存在
inline int CToolhelp::HowManyBlocksInHeap(DWORD dwProcessID, 
   DWORD dwHeapID) const {

   int nHowManyBlocksInHeap = 0;
   HEAPENTRY32 he = { sizeof(he) };
   BOOL fOk = HeapFirst(&he, dwProcessID, dwHeapID);
   for (; fOk; fOk = HeapNext(&he))
      nHowManyBlocksInHeap++;
   return(nHowManyBlocksInHeap);
}

// 遍历系统当前堆快照
inline BOOL CToolhelp::HeapListFirst(PHEAPLIST32 phl) const {

   return(Heap32ListFirst(m_hSnapshot, phl));
}


inline BOOL CToolhelp::HeapListNext(PHEAPLIST32 phl) const {

   return(Heap32ListNext(m_hSnapshot, phl));
}

// 实时遍历指定pid的进程虚拟地址空间中的堆块
inline BOOL CToolhelp::HeapFirst(PHEAPENTRY32 phe, DWORD dwProcessID, 
   UINT_PTR dwHeapID) const {

   return(Heap32First(phe, dwProcessID, dwHeapID));
}


inline BOOL CToolhelp::HeapNext(PHEAPENTRY32 phe) const {

   return(Heap32Next(phe));
}

// 通过遍历当前系统堆快照以及实时遍历每个堆中的堆块信息
// 判断指定地址是否是在堆当中
inline BOOL CToolhelp::IsAHeap(HANDLE hProcess, PVOID pvBlock, 
   PDWORD pdwFlags) const {

   HEAPLIST32 hl = { sizeof(hl) };
   for (BOOL fOkHL = HeapListFirst(&hl); fOkHL; fOkHL = HeapListNext(&hl)) {
      HEAPENTRY32 he = { sizeof(he) };
      BOOL fOkHE = HeapFirst(&he, hl.th32ProcessID, hl.th32HeapID);
	  // 原来for循环还可以这样写
      for (; fOkHE; fOkHE = HeapNext(&he)) {
         MEMORY_BASIC_INFORMATION mbi;
         VirtualQueryEx(hProcess, (PVOID) he.dwAddress, &mbi, sizeof(mbi));
         if (chINRANGE(mbi.AllocationBase, pvBlock, 
            (PBYTE) mbi.AllocationBase + mbi.RegionSize)) {

            *pdwFlags = hl.dwFlags;
            return(TRUE);
         }
      }
   }
   return(FALSE);
}


//////////////////////////////// End of File //////////////////////////////////