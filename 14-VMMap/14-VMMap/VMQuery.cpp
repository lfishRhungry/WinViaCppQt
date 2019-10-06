#include "VMQuery.h"

// 辅助结构（区域信息）
typedef struct {
	SIZE_T RgnSize;
	DWORD  dwRgnStorage;
	DWORD  dwRgnBlocks;
	DWORD  dwRgnGuardBlks;
	BOOL   bRgnIsAStack;
} VMQUERY_HELP;


// 表示本CPU平台的分配粒度值 在第一次调用VMQuery函数时初始化
static DWORD gs_dwAllocGran = 0;


///////////////////////////////////////////////////////////////////////////////


// 遍历指定区域中的块来获取完整的区域信息
// 并返回VMQUERY_HELP结构信息
static BOOL VMQueryHelp(
	HANDLE hProcess,
	LPCVOID pvAddress,
	VMQUERY_HELP* pVMQHelp) {

	ZeroMemory(pVMQHelp, sizeof(*pVMQHelp));

	// 获取包含指定内存地址的区域起始地址
	// 这里使用的时VirtualQueryEx 可以用来查询其他进程的地址信息
	MEMORY_BASIC_INFORMATION mbi;
	BOOL bOk = (
		VirtualQueryEx(hProcess, pvAddress, &mbi, sizeof(mbi)) == sizeof(mbi));

	if (!bOk)
		return(bOk);

	// 从该分配区域基地址开始
	// 该值在循环时时不会变的（始终都在同一个区域内）
	PVOID pvRgnBaseAddress = mbi.AllocationBase;

	// 先从该区域第一个块开始
	PVOID pvAddressBlk = pvRgnBaseAddress;

	// 保存存储器类型
	pVMQHelp->dwRgnStorage = mbi.Type;

	// 开始循环块
	for (;;) {
		// 获取当前块信息
		bOk = (VirtualQueryEx(hProcess, pvAddressBlk, &mbi, sizeof(mbi))
			== sizeof(mbi));
		if (!bOk)
			break;

		// 通过该地址所在区域基地址来判断是否还在我们查询的区域当中
		if (mbi.AllocationBase != pvRgnBaseAddress)
			break;

		// 开始处理块信息

		pVMQHelp->dwRgnBlocks++;             // 增加区域内块数量信息
		pVMQHelp->RgnSize += mbi.RegionSize; // 将改块大小加入到区域总大小当中

		// 如果该块有PAGE_GUARD属性 计数加1
		if ((mbi.Protect & PAGE_GUARD) == PAGE_GUARD)
			pVMQHelp->dwRgnGuardBlks++;

		// 通过块地址更加准确地猜测该区域存储器类型
		if (pVMQHelp->dwRgnStorage == MEM_PRIVATE)
			pVMQHelp->dwRgnStorage = mbi.Type;

		// 获取下一个块地址
		pvAddressBlk = (PVOID)((PBYTE)pvAddressBlk + mbi.RegionSize);
	}

	// 遍历完该区域后 猜测是否时线程栈
	pVMQHelp->bRgnIsAStack = (pVMQHelp->dwRgnGuardBlks > 0);

	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


// 重头戏
BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ) {

	if (gs_dwAllocGran == 0) {
		// 在第一次调用函数时设置分配粒度
		SYSTEM_INFO sinf;
		GetSystemInfo(&sinf);
		gs_dwAllocGran = sinf.dwAllocationGranularity;
	}

	ZeroMemory(pVMQ, sizeof(*pVMQ));

	// 获取传入地址的MEMORY_BASIC_INFORMATION信息
	MEMORY_BASIC_INFORMATION mbi;
	BOOL bOk = (VirtualQueryEx(hProcess, pvAddress, &mbi, sizeof(mbi))
		== sizeof(mbi));

	if (!bOk)
		return(bOk);

	// 准备就绪 开始填充我们的VMQUERY结构信息

	// 首先 填充块信息 之后再填充区域信息
	// 判断存储类型
	switch (mbi.State) {
	case MEM_FREE:       // 未预定
		pVMQ->pvBlkBaseAddress = NULL;
		pVMQ->BlkSize = 0;
		pVMQ->dwBlkProtection = 0;
		pVMQ->dwBlkStorage = MEM_FREE;
		break;

	case MEM_RESERVE:    // 预定但是未调拨
		pVMQ->pvBlkBaseAddress = mbi.BaseAddress;
		pVMQ->BlkSize = mbi.RegionSize;

		// 对于未调拨块 其保护属性是无效值
		// 所以填入其所在区域的保护属性
		pVMQ->dwBlkProtection = mbi.AllocationProtect;
		pVMQ->dwBlkStorage = MEM_RESERVE;
		break;

	case MEM_COMMIT:     // 已调拨
		pVMQ->pvBlkBaseAddress = mbi.BaseAddress;
		pVMQ->BlkSize = mbi.RegionSize;
		pVMQ->dwBlkProtection = mbi.Protect;
		pVMQ->dwBlkStorage = mbi.Type;
		break;

	default:
		DebugBreak();
		break;
	}

	// 现在开始填充区域信息
	// 利用上面定义的辅助函数
	VMQUERY_HELP VMQHelp;
	switch (mbi.State) {
	case MEM_FREE:       // 未预订处理方式
		pVMQ->pvRgnBaseAddress = mbi.BaseAddress;
		pVMQ->dwRgnProtection = mbi.AllocationProtect;
		pVMQ->RgnSize = mbi.RegionSize;
		pVMQ->dwRgnStorage = MEM_FREE;
		pVMQ->dwRgnBlocks = 0;
		pVMQ->dwRgnGuardBlks = 0;
		pVMQ->bRgnIsAStack = FALSE;
		break;

	case MEM_RESERVE:    // 已预订
		pVMQ->pvRgnBaseAddress = mbi.AllocationBase;
		pVMQ->dwRgnProtection = mbi.AllocationProtect;

		// 遍历区域内块来获取区域完整信息
		// 所以这个所谓的VirtualQuery系列函数获取的基本都是块信息
		VMQueryHelp(hProcess, pvAddress, &VMQHelp);

		pVMQ->RgnSize = VMQHelp.RgnSize;
		pVMQ->dwRgnStorage = VMQHelp.dwRgnStorage;
		pVMQ->dwRgnBlocks = VMQHelp.dwRgnBlocks;
		pVMQ->dwRgnGuardBlks = VMQHelp.dwRgnGuardBlks;
		pVMQ->bRgnIsAStack = VMQHelp.bRgnIsAStack;
		break;

	case MEM_COMMIT:     // 已预定
		pVMQ->pvRgnBaseAddress = mbi.AllocationBase;
		pVMQ->dwRgnProtection = mbi.AllocationProtect;

		// 遍历区域内块来获取区域完整信息
		VMQueryHelp(hProcess, pvAddress, &VMQHelp);

		pVMQ->RgnSize = VMQHelp.RgnSize;
		pVMQ->dwRgnStorage = VMQHelp.dwRgnStorage;
		pVMQ->dwRgnBlocks = VMQHelp.dwRgnBlocks;
		pVMQ->dwRgnGuardBlks = VMQHelp.dwRgnGuardBlks;
		pVMQ->bRgnIsAStack = VMQHelp.bRgnIsAStack;
		break;

	default:
		DebugBreak();
		break;
	}

	return(bOk);
}
