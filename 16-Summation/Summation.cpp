#include "Summation.h"

Summation::Summation(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//--------------------界面设计-----------------------------
	this->setWindowTitle("Summation");
	this->setFixedSize(700, 250);

	ui.labelRequest->setText("Calculate the sum of the numbers\nfrom 0 through x, where x is:");
	ui.lineEdit->setFixedWidth(150);
	ui.lineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.lineEdit->setValidator(new QIntValidator(this));

	ui.buttonCalc->setText("Calculate");
	ui.buttonCalc->setEnabled(false);
	ui.labelAnswer->setText("Answer:");
	ui.labelOutput->setFixedWidth(200);
	ui.labelOutput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui.labelOutput->setText("???");
	// -------------------界面设计完毕--------------------------

	// lineEdit处理逻辑
	connect(ui.lineEdit, &QLineEdit::textChanged, [=](const QString& text) {
		if (text.isEmpty()) {
			ui.buttonCalc->setEnabled(false);
		}
		else
		{
			ui.buttonCalc->setEnabled(true);
		}
		});

	// calculate计算逻辑
	connect(ui.buttonCalc, &QPushButton::clicked, [=]() {

		// 获取用户输入
		UINT uSum = ui.lineEdit->text().toUInt();

		// 创建线程来计算累加和
		DWORD dwThreadId;
		HANDLE hThread = chBEGINTHREADEX(NULL, 0,
			SumThreadFunc, (PVOID)(UINT_PTR)uSum, 0, &dwThreadId);

		// 等待函数
		WaitForSingleObject(hThread, INFINITE);

		// 线程的退出码就是计算和
		GetExitCodeThread(hThread, (PDWORD)&uSum);

		// 关闭线程内核对象
		CloseHandle(hThread);

		// 根据返回码进行逻辑处理
		if (uSum == UINT_MAX) {
			// 如果结果是UINT_MAX，说明线程栈溢出了
			ui.labelOutput->setText("Error");
			QMessageBox::warning(this, "Error", "The number is too big, please enter a smaller number");
		}
		else {
			// 计算成功
			ui.labelOutput->setText(QString::number(uSum, 10));
		}
		});
}

// 计算函数
UINT Sum(UINT uNum) {

	// 使用递归计算
	return((uNum == 0) ? 0 : (uNum + Sum(uNum - 1)));
}

// 设置SEH相关 过滤函数
LONG WINAPI FilterFunc(DWORD dwExceptionCode) {

	return((dwExceptionCode == STATUS_STACK_OVERFLOW)
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH);
}

// 单独使用线程来计算 为了更完整地使用1MB线程栈空间
// 同时 线程栈也只能被识别溢出一次 单独开线程可以实现程序内多次演示计算
DWORD WINAPI SumThreadFunc(PVOID pvParam) {

	// 给线程函数传入的参数就是要计算的值
	UINT uSumNum = PtrToUlong(pvParam);

	// nSum保存计算结果，如果无法算出 就返回最大uint值
	UINT uSum = UINT_MAX;

	// 使用SEH 捕捉线程栈溢出
	__try {
		uSum = Sum(uSumNum);
	}
	__except (FilterFunc(GetExceptionCode())) {
		// 如果代码运行到这里 说明栈空间溢出了
		// 在这里可以做任何能够保持程序优雅运行的事情
		// 不过对于本程序而言 无事可做
	}

	// The thread's exit code is the sum of the first uSumNum 
	// numbers, or UINT_MAX if a stack overflow occurred.
	// 此线程的退出码就是计算结果
	// 如果线程栈溢出了 返回的就是UINT_MAX
	return(uSum);
}
