#include "Summation.h"

Summation::Summation(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//--------------------�������-----------------------------
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
	// -------------------����������--------------------------

	// lineEdit�����߼�
	connect(ui.lineEdit, &QLineEdit::textChanged, [=](const QString& text) {
		if (text.isEmpty()) {
			ui.buttonCalc->setEnabled(false);
		}
		else
		{
			ui.buttonCalc->setEnabled(true);
		}
		});

	// calculate�����߼�
	connect(ui.buttonCalc, &QPushButton::clicked, [=]() {

		// ��ȡ�û�����
		UINT uSum = ui.lineEdit->text().toUInt();

		// �����߳��������ۼӺ�
		DWORD dwThreadId;
		HANDLE hThread = chBEGINTHREADEX(NULL, 0,
			SumThreadFunc, (PVOID)(UINT_PTR)uSum, 0, &dwThreadId);

		// �ȴ�����
		WaitForSingleObject(hThread, INFINITE);

		// �̵߳��˳�����Ǽ����
		GetExitCodeThread(hThread, (PDWORD)&uSum);

		// �ر��߳��ں˶���
		CloseHandle(hThread);

		// ���ݷ���������߼�����
		if (uSum == UINT_MAX) {
			// ��������UINT_MAX��˵���߳�ջ�����
			ui.labelOutput->setText("Error");
			QMessageBox::warning(this, "Error", "The number is too big, please enter a smaller number");
		}
		else {
			// ����ɹ�
			ui.labelOutput->setText(QString::number(uSum, 10));
		}
		});
}

// ���㺯��
UINT Sum(UINT uNum) {

	// ʹ�õݹ����
	return((uNum == 0) ? 0 : (uNum + Sum(uNum - 1)));
}

// ����SEH��� ���˺���
LONG WINAPI FilterFunc(DWORD dwExceptionCode) {

	return((dwExceptionCode == STATUS_STACK_OVERFLOW)
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH);
}

// ����ʹ���߳������� Ϊ�˸�������ʹ��1MB�߳�ջ�ռ�
// ͬʱ �߳�ջҲֻ�ܱ�ʶ�����һ�� �������߳̿���ʵ�ֳ����ڶ����ʾ����
DWORD WINAPI SumThreadFunc(PVOID pvParam) {

	// ���̺߳�������Ĳ�������Ҫ�����ֵ
	UINT uSumNum = PtrToUlong(pvParam);

	// nSum���������������޷���� �ͷ������uintֵ
	UINT uSum = UINT_MAX;

	// ʹ��SEH ��׽�߳�ջ���
	__try {
		uSum = Sum(uSumNum);
	}
	__except (FilterFunc(GetExceptionCode())) {
		// ����������е����� ˵��ջ�ռ������
		// ������������κ��ܹ����ֳ����������е�����
		// �������ڱ�������� ���¿���
	}

	// The thread's exit code is the sum of the first uSumNum 
	// numbers, or UINT_MAX if a stack overflow occurred.
	// ���̵߳��˳�����Ǽ�����
	// ����߳�ջ����� ���صľ���UINT_MAX
	return(uSum);
}
