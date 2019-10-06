#include "VMAlloc.h"

VMAlloc::VMAlloc(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// ------------------------------界面设计---------------------------------------------
	this->setFixedSize(1000, 400);
	this->setWindowTitle("Virtual Memory Allocator");

	ui.buttonReserve->setText("Reserve region (50 * 2KB structures)");

	ui.labelIndex->setText("Index (0-49):");
	ui.lineEdit->setValidator(new QIntValidator(this));
	ui.lineEdit->setFixedWidth(50);
	ui.buttonUse->setText("Use");
	ui.buttonUse->setFixedWidth(70);
	ui.buttonClear->setText("Clear");
	ui.buttonClear->setFixedWidth(90);
	ui.buttonCollect->setText("Garbage collect");

	ui.labelWhite->setText("White:Free");
	ui.labelGray->setText("Gray:Reserved");
	ui.labelBlack->setText("Black:Committed");

	ui.labelWhite->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.labelGray->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	ui.labelBlack->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

	//通过手动添加label数组来模拟内存映射图（心累）
	QLabel* labelMap[25] = {
		ui.label_0, ui.label_1, ui.label_2, ui.label_3, ui.label_4,
		ui.label_5, ui.label_6, ui.label_7, ui.label_8, ui.label_9,
		ui.label_10, ui.label_11, ui.label_12, ui.label_13, ui.label_14,
		ui.label_15, ui.label_16, ui.label_17, ui.label_18, ui.label_19,
		ui.label_20, ui.label_21, ui.label_22, ui.label_23, ui.label_24 };



	// 获取CPU页大小
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	g_uPageSize = si.dwPageSize;
	// 根据页面大小设置显示
	TCHAR szBuf[10];
	StringCchPrintf(szBuf, _countof(szBuf), TEXT("%d KB"), g_uPageSize / 1024);
	ui.labelPage->setText("Page: " + QString::fromWCharArray(szBuf));
	//-----------------------------------页面设计结束--------------------------

	// 初始状态设置
	ui.labelIndex->setEnabled(false);
	ui.lineEdit->setEnabled(false);
	ui.buttonUse->setEnabled(false);
	ui.buttonClear->setEnabled(false);
	ui.buttonCollect->setEnabled(false);

	for (int i = 0; i < 25; i++) {
		labelMap[i]->setFrameShape(QFrame::Box);
		labelMap[i]->setStyleSheet("background-color:white");
		labelMap[i]->setText("");
	}

	// Reserve按键
	connect(ui.buttonReserve, &QPushButton::clicked, [=]() {
		// 预订存放结构数组的虚拟地址空间
		g_pSomeData =
			(PSOMEDATA)VirtualAlloc(NULL, MAX_SOMEDATA * sizeof(SOMEDATA), MEM_RESERVE, PAGE_READWRITE);

		ui.buttonReserve->setEnabled(false);
		ui.lineEdit->setEnabled(true);
		ui.labelIndex->setEnabled(true);
		ui.buttonUse->setEnabled(true);
		ui.buttonCollect->setEnabled(true);

		// 强制将焦点放在lineEdit
		ui.lineEdit->setFocus();

		// 更新map示意图
		for (int i = 0; i < 25; i++) {
			labelMap[i]->setFrameShape(QFrame::Box);
			labelMap[i]->setStyleSheet("background-color:gray");
			labelMap[i]->setText("");
		}
		});

	// 处理index输入
	connect(ui.lineEdit, &QLineEdit::textChanged, [=]() {

		bool ok; // 转换成功检查标志
		g_uIndex = ui.lineEdit->text().toUInt(&ok, 10);
		// 已预订虚拟地址且输入合理
		if ((g_pSomeData != NULL) && chINRANGE(0, g_uIndex, MAX_SOMEDATA - 1)) {
			// 如果该索引所指结构所在页面已调拨
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery(&g_pSomeData[g_uIndex], &mbi, sizeof(mbi));
			BOOL bOk = (mbi.State == MEM_COMMIT);
			if (bOk)
				bOk = g_pSomeData[g_uIndex].bInUse;

			// 然后再根据该索引所指结构是否在使用来设定按键
			ui.buttonUse->setEnabled(!bOk);
			ui.buttonClear->setEnabled(bOk);
		}
		else {
			// 否则都禁止
			ui.buttonUse->setEnabled(false);
			ui.buttonClear->setEnabled(false);
		}
		});

	// 点击use按钮效果
	connect(ui.buttonUse, &QPushButton::clicked, [=]() {

		// 由于上面的槽连接已经检查过g_uIndex全局变量，这里可以直接使用
		// 注意 新调拨页面自动被系统置零
		VirtualAlloc(&g_pSomeData[g_uIndex], sizeof(SOMEDATA),
			MEM_COMMIT, PAGE_READWRITE);

		g_pSomeData[g_uIndex].bInUse = TRUE;

		ui.buttonUse->setEnabled(false);
		ui.buttonClear->setEnabled(true);

		// 强制移动过焦点到clear按键
		ui.buttonClear->setFocus();

		// 更新map示意图
		labelMap[g_uIndex / 2]->setStyleSheet("background-color:black");
		});

	// Clear键
	connect(ui.buttonClear, &QPushButton::clicked, [=]() {

		// 由于上面的槽连接已经检查过g_uIndex全局变量，这里可以直接使用
		g_pSomeData[g_uIndex].bInUse = FALSE;
		ui.buttonUse->setEnabled(true);
		ui.buttonClear->setEnabled(false);

		// 移动焦点到use按键
		ui.buttonUse->setFocus();
		});

	// collect按键
	// 回收包含结构已被清理的页面
	// 可移植性强
	connect(ui.buttonCollect, &QPushButton::clicked, [=]() {

		UINT uMaxPages = MAX_SOMEDATA * sizeof(SOMEDATA) / g_uPageSize;
		// 以每一页为基础来遍历
		for (UINT uPage = 0; uPage < uMaxPages; uPage++) {
			BOOL bAnyAllocsInThisPage = FALSE;
			UINT uIndex = uPage * g_uPageSize / sizeof(SOMEDATA);
			UINT uIndexLast = uIndex + g_uPageSize / sizeof(SOMEDATA);

			// 以本页中的每一个结构为基础遍历
			for (; uIndex < uIndexLast; uIndex++) {
				MEMORY_BASIC_INFORMATION mbi;
				VirtualQuery(&g_pSomeData[uIndex], &mbi, sizeof(mbi));
				// 当该页是已调拨状态
				// 且该结构的第一个成员（标记是否在使用）显示此结构正在使用
				// 则标记此页面为 有分配项的页面
				bAnyAllocsInThisPage = (
					(mbi.State == MEM_COMMIT) && *(PBOOL)((PBYTE)g_pSomeData + sizeof(SOMEDATA) * uIndex));

				// Stop checking this page, we know we can't decommit it.
				// 如果本页面是有分配项的就不用看这个页面了 因为我们怎么也不能对这个页面取消调拨
				if (bAnyAllocsInThisPage) break;
			}

			if (!bAnyAllocsInThisPage) {
				// 如果该页面没有已分配项 就取消调拨
				// 这里取消调拨是以该页面最后一个结构地址取消的
				VirtualFree(&g_pSomeData[uIndexLast - 1], sizeof(SOMEDATA), MEM_DECOMMIT);

				// 这里根据本程序特性，多加了更新map示意图的语句
				labelMap[uPage]->setStyleSheet("background-color:gray");

			}
		}

		// 更新map示意图
		});
}

// 这里主要是释放空间
VMAlloc::~VMAlloc() {

	if (g_pSomeData != NULL)
		VirtualFree(g_pSomeData, 0, MEM_RELEASE);
}

