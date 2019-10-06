#include "Batch.h"

// ����Ҫ���̺߳���ʹ�� �����������ⲿ
HWND     g_hDlg = NULL; // �����ھ��
volatile LONG g_nCurrentTask = 0; // ������������ı�־

// �̺߳���
void NTAPI TaskHandler(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);


Batch::Batch(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//--------------------------�������---------------------------------
	this->setWindowTitle("Batch processing with thread pool");

	this->setMinimumSize(700, 600);

	ui.plainTextEdit->setReadOnly(true);

	ui.buttonStart->setText("Start batch");

	ui.buttonExit->setText("Exit");
	//--------------------------����������---------------------------------

	g_hDlg = (HWND)winId(); // ��ȡ���ھ��

	   // Create the work item that will be used by all tasks
	g_pWorkItem = CreateThreadpoolWork(TaskHandler, NULL, NULL);

	if (g_pWorkItem == NULL) {
		QMessageBox::critical(this, "Error", "Impossible to create the work item for tasks.");
		this->close();
	}

	// ���exit�رճ���
	connect(ui.buttonExit, &QPushButton::clicked, this, &QWidget::close);

	// ���start
	connect(ui.buttonStart, &QPushButton::clicked, [=]() {
		// ��ֹ�ظ��ύ
		ui.buttonStart->setEnabled(false);

		ui.plainTextEdit->appendPlainText("----Start a new batch----");;

		// ʹ��ͬһ���������ύ4������
		// �ύ�����̳߳�
		SubmitThreadpoolWork(g_pWorkItem);
		SubmitThreadpoolWork(g_pWorkItem);
		SubmitThreadpoolWork(g_pWorkItem);
		SubmitThreadpoolWork(g_pWorkItem);

		ui.plainTextEdit->appendPlainText("4 tasks are submitted.");
		});
}

Batch::~Batch() {
	// �ǵùص���
	if (g_pWorkItem != NULL) {
		CloseThreadpoolWork(g_pWorkItem);
	}
}


// ��Ϣ������
bool Batch::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	MSG* msg = (MSG*)message;

	switch (msg->message)
	{
	case WM_COPYDATA: {
		// �յ����̵߳���Ϣ
		PCOPYDATASTRUCT pcds = reinterpret_cast<PCOPYDATASTRUCT>(msg->lParam);
		ui.plainTextEdit->appendPlainText(QString::fromWCharArray((PCWSTR)pcds->lpData));
		break;
	}

	case WM_APP_COMPLETED: {
		// �յ�ȫ��������ɵ���Ϣ
		TCHAR szMsg[MAX_PATH + 1];
		StringCchPrintf(
			szMsg, _countof(szMsg),
			TEXT("_Task #%u was the last task of the batch_"), msg->lParam);

		ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szMsg));

		// �����ٴο���start��
		ui.buttonStart->setEnabled(true);
		break;
	}

	default:
		break;
	}


	return false;
}

// �̺߳���
// ע������̺߳����Ǿ����ϸ�ĺ���ԭ�Ͷ����
// ���Բ�����QWidget�ĳ�Ա��������Ա�����Դ�һ��thisָ�������
// ֻ���������ⲿ���ܷ�������������
// �������������Ҫ�����ڽ��� ֻ��ͨ������WM_COPYDATA��
void NTAPI TaskHandler(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work) {

	// ���ӹ�����
	LONG currentTask = InterlockedIncrement(&g_nCurrentTask);

	TCHAR szMsg[MAX_PATH];
	StringCchPrintf(
		szMsg, _countof(szMsg),
		TEXT("[%u] Task #%u is starting."), GetCurrentThreadId(), currentTask);

	// ͨ������WM_COPYDATA��Ϣ��֪ͨui����
	COPYDATASTRUCT cds = { 0, ((DWORD)wcslen(szMsg) + 1) * sizeof(wchar_t), szMsg };
	FORWARD_WM_COPYDATA(g_hDlg, NULL, &cds, SendMessage);

	// ģ�����ڹ���
	Sleep(currentTask * 1000);

	StringCchPrintf(
		szMsg, _countof(szMsg),
		TEXT("[%u] Task #%u is done."), GetCurrentThreadId(), currentTask);

	// ͨ������WM_COPYDATA��Ϣ��֪ͨui����
	cds = { 0, ((DWORD)wcslen(szMsg) + 1) * sizeof(wchar_t), szMsg };
	FORWARD_WM_COPYDATA(g_hDlg, NULL, &cds, SendMessage);

	if (InterlockedDecrement(&g_nCurrentTask) == 0)
	{
		// ����ui�����������������
		// �����Լ���task����Ϊ����
		PostMessage(g_hDlg, WM_APP_COMPLETED, 0, (LPARAM)currentTask);
	}
}
