// Module:  Toolhelp.h
// ����Jeffrey Richter & Christophe Nasarre��ʵ�õ�C++��
// ��toolhelpϵ�к�����һЩ��װ
// ��Ҫ�����У�
// ��������õ�ǰ���̵�ָ��Ȩ��
// ��ȡָ������ָ���ڴ��ַ��ָ����С����

// ����ϵͳ��ǰ���̿���
// ͨ�����̿��պ�pidѰ��ָ�����̵�PROCESSENTRY32�ṹ��Ϣ

// ����ϵͳ��ǰģ�����
// ͨ��ģ���ַ��module�� Ѱ��ָ��ģ���MODULEENTRY32�ṹ��Ϣ
// ͨ��ģ������ Ѱ��ָ��ģ���MODULEENTRY32�ṹ��Ϣ

// ����ϵͳ��ǰ�߳̿���

// ͨ����ǰϵͳ�ѿ��� �����ж��ٸ��Ѵ���
// ͨ��ʵʱ����ָ��pid���̵������ַ�ռ� �����ж��ٶѿ����
// ����ϵͳ��ǰ�ѿ���
// ʵʱ����ָ��pid�Ľ��������ַ�ռ��еĶѿ�

// ͨ��������ǰϵͳ�ѿ����Լ�ʵʱ����ÿ�����еĶѿ���Ϣ �ж�ָ����ַ�Ƿ����ڶѵ���


// ��Ϊͷ�ļ���ʹ�� ֻ��������һ��
#pragma once


///////////////////////////////////////////////////////////////////////////////


#include "CmnHdr.h"
#include <tlhelp32.h>
#include <tchar.h>


///////////////////////////////////////////////////////////////////////////////

// ע�⣺C++���к��������������const�ؼ��ֱ�ʾ
// �������߼�������ı���������ĳ�Ա������ֵ
// �����ĺ�����Ϊ����Ա����


class CToolhelp {
private:
   HANDLE m_hSnapshot;

public:
   CToolhelp(DWORD dwFlags = 0, DWORD dwProcessID = 0);
   ~CToolhelp();
   // �������
   BOOL CreateSnapshot(DWORD dwFlags, DWORD dwProcessID = 0);
   // ���̿������
   BOOL ProcessFirst(PPROCESSENTRY32 ppe) const;
   BOOL ProcessNext(PPROCESSENTRY32 ppe) const;
   BOOL ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const;
   // ģ��������
   BOOL ModuleFirst(PMODULEENTRY32 pme) const;
   BOOL ModuleNext(PMODULEENTRY32 pme) const;
   BOOL ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const;
   BOOL ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const;
   // �߳̿������
   BOOL ThreadFirst(PTHREADENTRY32 pte) const;
   BOOL ThreadNext(PTHREADENTRY32 pte) const;
   // �ѱ���
   BOOL HeapListFirst(PHEAPLIST32 phl) const;
   BOOL HeapListNext(PHEAPLIST32 phl) const;
   int  HowManyHeaps() const;

   // ע�⣺�������ڶѿ�����ĺ���������ʹ�ÿ��ռ���ʵ�ֵ�
   // ÿ�ζ�����ֱ�ӱ����������еĽ��̵������ַ�ռ��еĶѿ�
   // ���� �������һ��������� ���ǵ��������ڱ����ѿ��ʱ��
   // ���̱���Ҳ�ڸı�ѿ� ����п��ܵ������ǵı�����Ϊ��ѭ��
   BOOL HeapFirst(PHEAPENTRY32 phe, DWORD dwProcessID, 
      UINT_PTR dwHeapID) const;
   BOOL HeapNext(PHEAPENTRY32 phe) const;
   int  HowManyBlocksInHeap(DWORD dwProcessID, DWORD dwHeapId) const;
   BOOL IsAHeap(HANDLE hProcess, PVOID pvBlock, PDWORD pdwFlags) const;

public:
   // ��������ý��̵�ָ��Ȩ��
   static BOOL EnablePrivilege(PCTSTR szPrivilege, BOOL fEnable = TRUE);
   // ��ȡָ������ָ���ڴ��ַ��ָ����С����
   static BOOL ReadProcessMemory(DWORD dwProcessID, LPCVOID pvBaseAddress, 
      PVOID pvBuffer, SIZE_T cbRead, SIZE_T* pNumberOfBytesRead = NULL);
};

///////////////////////////////////////////////////////////////////////////////
/*�������еĺ�����ʹ��ʱ������Ҫ��Ƶ������
Ϊ�˼���ռ�ռ������ ������������ʽ*/
///////////////////////////////////////////////////////////////////////////////


inline CToolhelp::CToolhelp(DWORD dwFlags, DWORD dwProcessID) {

   m_hSnapshot = INVALID_HANDLE_VALUE; // �Ƚ����վ����Ϊ�Ƿ�ֵ
   CreateSnapshot(dwFlags, dwProcessID);
}


///////////////////////////////////////////////////////////////////////////////


inline CToolhelp::~CToolhelp() {

   if (m_hSnapshot != INVALID_HANDLE_VALUE)
      CloseHandle(m_hSnapshot); // ���پ��
}


///////////////////////////////////////////////////////////////////////////////


inline BOOL CToolhelp::CreateSnapshot(DWORD dwFlags, DWORD dwProcessID) {

   if (m_hSnapshot != INVALID_HANDLE_VALUE) // ȷ����Ҫ�����κ��ں˶�����
      CloseHandle(m_hSnapshot);

   if (dwFlags == 0) {
      m_hSnapshot = INVALID_HANDLE_VALUE; // û�д���Ϸ�flagֵ
   } else {
      m_hSnapshot = CreateToolhelp32Snapshot(dwFlags, dwProcessID); // ����Ҫ���������
   }
   return(m_hSnapshot != INVALID_HANDLE_VALUE);
}


///////////////////////////////////////////////////////////////////////////////

// ��������ý��̵�ָ��Ȩ��
inline BOOL CToolhelp::EnablePrivilege(PCTSTR szPrivilege, BOOL fEnable) {

   // ͨ����������Ȩ�� ʹ������Բ鿴����������Ϣ
   BOOL fOk = FALSE;    // ����Ȩ������ʧ��
   HANDLE hToken;

   // ���������̵����� ָ�����ǵĴ�Ȩ��Ϊ ���Ե���ָ������Ȩ�� ��Ȩ��
   if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {

      // ����һ��TOKEN_PRIVILEGES�ṹ
	  // ���ú�Ҫ�޸ĵ���Ϣ
      TOKEN_PRIVILEGES tp;
      tp.PrivilegeCount = 1;
	  // �õ���Ҫ������Ȩ�޵�LUID
      LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
      tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0; // ����ָ��Ȩ�޵Ŀ����ͽ���
	  // ����ṹ �޸�����Ȩ��
      AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
      fOk = (GetLastError() == ERROR_SUCCESS);

      // �رվ��
      CloseHandle(hToken);
   }
   return(fOk);
}


///////////////////////////////////////////////////////////////////////////////

// ��ȡָ������ָ���ڴ��ַ��ָ����С����
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
      fOk = ProcessNext(ppe); // ����pidΪ0��ϵͳ���н��̣�system idle process��
   return(fOk);
}


inline BOOL CToolhelp::ProcessNext(PPROCESSENTRY32 ppe) const {

   BOOL fOk = Process32Next(m_hSnapshot, ppe);
   if (fOk && (ppe->th32ProcessID == 0))
      fOk = ProcessNext(ppe); // ����pidΪ0��ϵͳ���н��̣�system idle process��
   return(fOk);
}


inline BOOL CToolhelp::ProcessFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) 
   const {

   BOOL fFound = FALSE;
   // ����forѭ�� ͨ��pidѰ��ָ������PROCESSENTRY32�ṹ
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

// ͨ�����ػ���ַ��module��Ѱ��ָ��ģ���MODULEENTRY32�ṹ��Ϣ
inline BOOL CToolhelp::ModuleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {

   BOOL fFound = FALSE;
   for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
      fFound = (pme->modBaseAddr == pvBaseAddr);
      if (fFound) break;
   }
   return(fFound);
}

// ͨ��ģ������ Ѱ��ָ��ģ���MODULEENTRY32�ṹ��Ϣ
inline BOOL CToolhelp::ModuleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
   BOOL fFound = FALSE;
   for (BOOL fOk = ModuleFirst(pme); fOk; fOk = ModuleNext(pme)) {
	  // ����ͬʱ�Ա���ָ���ַ����͵�ǰ����ģ��������Լ�·����Ϣ
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

// ͨ����ǰϵͳ�ѿ��� �����ж��ٸ��Ѵ���
inline int CToolhelp::HowManyHeaps() const {

   int nHowManyHeaps = 0;
   HEAPLIST32 hl = { sizeof(hl) };
   for (BOOL fOk = HeapListFirst(&hl); fOk; fOk = HeapListNext(&hl))
      nHowManyHeaps++;
   return(nHowManyHeaps);
}

// ͨ��ʵʱ����ָ��pid���̵������ַ�ռ� �����ж��ٶѿ����
inline int CToolhelp::HowManyBlocksInHeap(DWORD dwProcessID, 
   DWORD dwHeapID) const {

   int nHowManyBlocksInHeap = 0;
   HEAPENTRY32 he = { sizeof(he) };
   BOOL fOk = HeapFirst(&he, dwProcessID, dwHeapID);
   for (; fOk; fOk = HeapNext(&he))
      nHowManyBlocksInHeap++;
   return(nHowManyBlocksInHeap);
}

// ����ϵͳ��ǰ�ѿ���
inline BOOL CToolhelp::HeapListFirst(PHEAPLIST32 phl) const {

   return(Heap32ListFirst(m_hSnapshot, phl));
}


inline BOOL CToolhelp::HeapListNext(PHEAPLIST32 phl) const {

   return(Heap32ListNext(m_hSnapshot, phl));
}

// ʵʱ����ָ��pid�Ľ��������ַ�ռ��еĶѿ�
inline BOOL CToolhelp::HeapFirst(PHEAPENTRY32 phe, DWORD dwProcessID, 
   UINT_PTR dwHeapID) const {

   return(Heap32First(phe, dwProcessID, dwHeapID));
}


inline BOOL CToolhelp::HeapNext(PHEAPENTRY32 phe) const {

   return(Heap32Next(phe));
}

// ͨ��������ǰϵͳ�ѿ����Լ�ʵʱ����ÿ�����еĶѿ���Ϣ
// �ж�ָ����ַ�Ƿ����ڶѵ���
inline BOOL CToolhelp::IsAHeap(HANDLE hProcess, PVOID pvBlock, 
   PDWORD pdwFlags) const {

   HEAPLIST32 hl = { sizeof(hl) };
   for (BOOL fOkHL = HeapListFirst(&hl); fOkHL; fOkHL = HeapListNext(&hl)) {
      HEAPENTRY32 he = { sizeof(he) };
      BOOL fOkHE = HeapFirst(&he, hl.th32ProcessID, hl.th32HeapID);
	  // ԭ��forѭ������������д
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