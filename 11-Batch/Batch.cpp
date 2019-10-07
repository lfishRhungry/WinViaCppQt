#include "Batch.h"

// 由于要被线程函数使用 所以声明在外部
HWND     g_hDlg = NULL; // 本窗口句柄
volatile LONG g_nCurrentTask = 0; // 完成任务数量的标志

// 线程函数
void NTAPI TaskHandler(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);


Batch::Batch(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//--------------------------界面设计---------------------------------
	this->setWindowTitle("Batch processing with thread pool");

	this->setMinimumSize(700, 600);

	ui.plainTextEdit->setReadOnly(true);

	ui.buttonStart->setText("Start batch");

	ui.buttonExit->setText("Exit");
	//--------------------------界面设计完毕---------------------------------

	g_hDlg = (HWND)winId(); // 获取窗口句柄

	   // Create the work item that will be used by all tasks
	g_pWorkItem = CreateThreadpoolWork(TaskHandler, NULL, NULL);

	if (g_pWorkItem == NULL) {
		QMessageBox::critical(this, "Error", "Impossible to create the work item for tasks.");
		this->close();
	}

	// 点击exit关闭程序
	connect(ui.buttonExit, &QPushButton::clicked, this, &QWidget::close);

	// 点击start
	connect(ui.buttonStart, &QPushButton::clicked, [=]() {
		// 防止重复提交
		ui.buttonStart->setEnabled(false);

		ui.plainTextEdit->appendPlainText("----Start a new batch----");;

		// 使用同一个工作项提交4个任务
		// 提交任务到线程池
		SubmitThreadpoolWork(g_pWorkItem);
		SubmitThreadpoolWork(g_pWorkItem);
		SubmitThreadpoolWork(g_pWorkItem);
		SubmitThreadpoolWork(g_pWorkItem);

		ui.plainTextEdit->appendPlainText("4 tasks are submitted.");
		});
}

Batch::~Batch() {
	// 记得关掉啊
	if (g_pWorkItem != NULL) {
		CloseThreadpoolWork(g_pWorkItem);
	}
}


// 消息处理函数
bool Batch::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	MSG* msg = (MSG*)message;

	switch (msg->message)
	{
	case WM_COPYDATA: {
		// 收到了线程的消息
		PCOPYDATASTRUCT pcds = reinterpret_cast<PCOPYDATASTRUCT>(msg->lParam);
		ui.plainTextEdit->appendPlainText(QString::fromWCharArray((PCWSTR)pcds->lpData));
		break;
	}

	case WM_APP_COMPLETED: {
		// 收到全部工作完成的消息
		TCHAR szMsg[MAX_PATH + 1];
		StringCchPrintf(
			szMsg, _countof(szMsg),
			TEXT("_Task #%u was the last task of the batch_"), msg->lParam);

		ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szMsg));

		// 可以再次开启start键
		ui.buttonStart->setEnabled(true);
		break;
	}

	default:
		break;
	}


	return false;
}

// 线程函数
// 注意这个线程函数是具有严格的函数原型定义的
// 所以不能是QWidget的成员函数（成员函数自带一个this指针参数）
// 只能声明在外部才能符合其声明定义
// 但是这个函数又要跟窗口交互 只能通过发送WM_COPYDATA了
void NTAPI TaskHandler(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work) {

	// 增加工作量
	LONG currentTask = InterlockedIncrement(&g_nCurrentTask);

	TCHAR szMsg[MAX_PATH];
	StringCchPrintf(
		szMsg, _countof(szMsg),
		TEXT("[%u] Task #%u is starting."), GetCurrentThreadId(), currentTask);

	// 通过发送WM_COPYDATA消息来通知ui窗口
	COPYDATASTRUCT cds = { 0, ((DWORD)wcslen(szMsg) + 1) * sizeof(wchar_t), szMsg };
	FORWARD_WM_COPYDATA(g_hDlg, NULL, &cds, SendMessage);

	// 模拟正在工作
	Sleep(currentTask * 1000);

	StringCchPrintf(
		szMsg, _countof(szMsg),
		TEXT("[%u] Task #%u is done."), GetCurrentThreadId(), currentTask);

	// 通过发送WM_COPYDATA消息来通知ui窗口
	cds = { 0, ((DWORD)wcslen(szMsg) + 1) * sizeof(wchar_t), szMsg };
	FORWARD_WM_COPYDATA(g_hDlg, NULL, &cds, SendMessage);

	if (InterlockedDecrement(&g_nCurrentTask) == 0)
	{
		// 告诉ui窗口所有任务已完成
		// 并将自己的task号作为参数
		PostMessage(g_hDlg, WM_APP_COMPLETED, 0, (LPARAM)currentTask);
	}
}
