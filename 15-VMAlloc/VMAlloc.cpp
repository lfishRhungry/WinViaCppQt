#include "VMAlloc.h"

VMAlloc::VMAlloc(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// ------------------------------�������---------------------------------------------
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

	//ͨ���ֶ����label������ģ���ڴ�ӳ��ͼ�����ۣ�
	QLabel* labelMap[25] = {
		ui.label_0, ui.label_1, ui.label_2, ui.label_3, ui.label_4,
		ui.label_5, ui.label_6, ui.label_7, ui.label_8, ui.label_9,
		ui.label_10, ui.label_11, ui.label_12, ui.label_13, ui.label_14,
		ui.label_15, ui.label_16, ui.label_17, ui.label_18, ui.label_19,
		ui.label_20, ui.label_21, ui.label_22, ui.label_23, ui.label_24 };



	// ��ȡCPUҳ��С
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	g_uPageSize = si.dwPageSize;
	// ����ҳ���С������ʾ
	TCHAR szBuf[10];
	StringCchPrintf(szBuf, _countof(szBuf), TEXT("%d KB"), g_uPageSize / 1024);
	ui.labelPage->setText("Page: " + QString::fromWCharArray(szBuf));
	//-----------------------------------ҳ����ƽ���--------------------------

	// ��ʼ״̬����
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

	// Reserve����
	connect(ui.buttonReserve, &QPushButton::clicked, [=]() {
		// Ԥ����Žṹ����������ַ�ռ�
		g_pSomeData =
			(PSOMEDATA)VirtualAlloc(NULL, MAX_SOMEDATA * sizeof(SOMEDATA), MEM_RESERVE, PAGE_READWRITE);

		ui.buttonReserve->setEnabled(false);
		ui.lineEdit->setEnabled(true);
		ui.labelIndex->setEnabled(true);
		ui.buttonUse->setEnabled(true);
		ui.buttonCollect->setEnabled(true);

		// ǿ�ƽ��������lineEdit
		ui.lineEdit->setFocus();

		// ����mapʾ��ͼ
		for (int i = 0; i < 25; i++) {
			labelMap[i]->setFrameShape(QFrame::Box);
			labelMap[i]->setStyleSheet("background-color:gray");
			labelMap[i]->setText("");
		}
		});

	// ����index����
	connect(ui.lineEdit, &QLineEdit::textChanged, [=]() {

		bool ok; // ת���ɹ�����־
		g_uIndex = ui.lineEdit->text().toUInt(&ok, 10);
		// ��Ԥ�������ַ���������
		if ((g_pSomeData != NULL) && chINRANGE(0, g_uIndex, MAX_SOMEDATA - 1)) {
			// �����������ָ�ṹ����ҳ���ѵ���
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery(&g_pSomeData[g_uIndex], &mbi, sizeof(mbi));
			BOOL bOk = (mbi.State == MEM_COMMIT);
			if (bOk)
				bOk = g_pSomeData[g_uIndex].bInUse;

			// Ȼ���ٸ��ݸ�������ָ�ṹ�Ƿ���ʹ�����趨����
			ui.buttonUse->setEnabled(!bOk);
			ui.buttonClear->setEnabled(bOk);
		}
		else {
			// ���򶼽�ֹ
			ui.buttonUse->setEnabled(false);
			ui.buttonClear->setEnabled(false);
		}
		});

	// ���use��ťЧ��
	connect(ui.buttonUse, &QPushButton::clicked, [=]() {

		// ��������Ĳ������Ѿ�����g_uIndexȫ�ֱ������������ֱ��ʹ��
		// ע�� �µ���ҳ���Զ���ϵͳ����
		VirtualAlloc(&g_pSomeData[g_uIndex], sizeof(SOMEDATA),
			MEM_COMMIT, PAGE_READWRITE);

		g_pSomeData[g_uIndex].bInUse = TRUE;

		ui.buttonUse->setEnabled(false);
		ui.buttonClear->setEnabled(true);

		// ǿ���ƶ������㵽clear����
		ui.buttonClear->setFocus();

		// ����mapʾ��ͼ
		labelMap[g_uIndex / 2]->setStyleSheet("background-color:black");
		});

	// Clear��
	connect(ui.buttonClear, &QPushButton::clicked, [=]() {

		// ��������Ĳ������Ѿ�����g_uIndexȫ�ֱ������������ֱ��ʹ��
		g_pSomeData[g_uIndex].bInUse = FALSE;
		ui.buttonUse->setEnabled(true);
		ui.buttonClear->setEnabled(false);

		// �ƶ����㵽use����
		ui.buttonUse->setFocus();
		});

	// collect����
	// ���հ����ṹ�ѱ������ҳ��
	// ����ֲ��ǿ
	connect(ui.buttonCollect, &QPushButton::clicked, [=]() {

		UINT uMaxPages = MAX_SOMEDATA * sizeof(SOMEDATA) / g_uPageSize;
		// ��ÿһҳΪ����������
		for (UINT uPage = 0; uPage < uMaxPages; uPage++) {
			BOOL bAnyAllocsInThisPage = FALSE;
			UINT uIndex = uPage * g_uPageSize / sizeof(SOMEDATA);
			UINT uIndexLast = uIndex + g_uPageSize / sizeof(SOMEDATA);

			// �Ա�ҳ�е�ÿһ���ṹΪ��������
			for (; uIndex < uIndexLast; uIndex++) {
				MEMORY_BASIC_INFORMATION mbi;
				VirtualQuery(&g_pSomeData[uIndex], &mbi, sizeof(mbi));
				// ����ҳ���ѵ���״̬
				// �Ҹýṹ�ĵ�һ����Ա������Ƿ���ʹ�ã���ʾ�˽ṹ����ʹ��
				// ���Ǵ�ҳ��Ϊ �з������ҳ��
				bAnyAllocsInThisPage = (
					(mbi.State == MEM_COMMIT) && *(PBOOL)((PBYTE)g_pSomeData + sizeof(SOMEDATA) * uIndex));

				// Stop checking this page, we know we can't decommit it.
				// �����ҳ�����з�����ľͲ��ÿ����ҳ���� ��Ϊ������ôҲ���ܶ����ҳ��ȡ������
				if (bAnyAllocsInThisPage) break;
			}

			if (!bAnyAllocsInThisPage) {
				// �����ҳ��û���ѷ����� ��ȡ������
				// ����ȡ���������Ը�ҳ�����һ���ṹ��ַȡ����
				VirtualFree(&g_pSomeData[uIndexLast - 1], sizeof(SOMEDATA), MEM_DECOMMIT);

				// ������ݱ��������ԣ�����˸���mapʾ��ͼ�����
				labelMap[uPage]->setStyleSheet("background-color:gray");

			}
		}

		// ����mapʾ��ͼ
		});
}

// ������Ҫ���ͷſռ�
VMAlloc::~VMAlloc() {

	if (g_pSomeData != NULL)
		VirtualFree(g_pSomeData, 0, MEM_RELEASE);
}

