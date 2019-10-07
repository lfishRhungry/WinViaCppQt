#include "MMFShare.h"

MMFShare::MMFShare(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//-------------------------�������------------------------------
	this->setWindowTitle("Memory-Mapped File Sharing");
	this->setFixedSize(800, 300);

	ui.buttonCreate->setText("Create mapping of Data");
	ui.buttonClose->setText("Close mapping of Data");

	ui.labelData->setText("Data:");
	ui.lineEdit->setText("Some test Data");

	ui.buttonMap->setFixedWidth(400);
	ui.buttonMap->setText("Open mapping and get Data");
	//-------------------------����������------------------------------

	ui.buttonClose->setEnabled(false);


	// ������ť���߼�
	connect(ui.buttonCreate, &QPushButton::clicked, [=]() {
		do
		{
			// ����һ�������ڴ�ӳ���ļ��ں˶���������lineEdit�ڵ�ֵ ����Ϊһҳ��С
			// ����INVALID_HANDLE_VALUE��ʾ��������洢��Ϊ�󱸴洢��
			// ��ҳ�����ļ�Ϊ�󱸴洢��
			m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
				PAGE_READWRITE, 0, 4 * 1024, TEXT("MMFSharedData"));

			if (m_hFileMap == NULL) {
				QMessageBox::critical(this, "Error", "Can't map view of file.");
				break;
			}

			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				chMB("Mapping already exists - not created.");
				CloseHandle(m_hFileMap);
				break;
			}

			// �������ڴ�ӳ���ļ������Ѿ������ɹ���
			// �����ҪԤ���͵���
			PVOID pView = MapViewOfFile(m_hFileMap,
				FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

			if (pView == NULL) {
				QMessageBox::critical(this, "Error", "Can't map view of file.");
				break;
			}

			// ���ı����ݸ��Ƶ��ڴ�ӳ���ļ���
			// ����ע��Ҫ�����ַ���sizeInWords
			_tcscpy_s(
				(PTSTR)pView, 
				4 * 1024 / sizeof(WCHAR), 
				reinterpret_cast<const wchar_t*>(ui.lineEdit->text().utf16()));

			// ȡ��ӳ�䣨����û�йر��ں˶��� ��Ϊ�˱����ڴ�ӳ���ļ���
			UnmapViewOfFile(pView);

			// ��ֹ�ٴδ����ļ�ӳ��
			ui.buttonCreate->setEnabled(false);

			// ��ֹ�ڹرձ����ڴ�ӳ�����֮ǰ�Ķ�����
			ui.lineEdit->setReadOnly(true);

			// �û����Թر��ڴ�ӳ���ļ�����
			ui.buttonClose->setEnabled(true);

		} while (false);

		});

	// �ر��ڴ�ӳ���ļ�����
	connect(ui.buttonClose, &QPushButton::clicked, [=]() {
		if (CloseHandle(m_hFileMap)) {
			ui.buttonCreate->setEnabled(true);
			ui.lineEdit->setReadOnly(false);
		}
		});

	// map�߼�
	connect(ui.buttonMap, &QPushButton::clicked, [=]() {

		// �鿴�Ƿ��������Ѿ������õ��ڴ�ӳ���ļ�����
		HANDLE hFileMapT = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
			FALSE, TEXT("MMFSharedData"));

		if (hFileMapT == NULL) {
			QMessageBox::critical(this, "Error", "Can't open mapping.");
			return;
		}

		// ��ȷʵ���ڵ��ڴ�ӳ���ļ�map���������ڴ�ռ�
		PVOID pView = MapViewOfFile(hFileMapT,
			FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

		if (pView == NULL) {
			QMessageBox::critical(this, "Error", "Can't map view.");
			return;
		}

		// ��ӳ�����������copy��lineedit����
		ui.lineEdit->setText(QString::fromWCharArray((PTSTR)pView));

		UnmapViewOfFile(pView); // ȡ��ӳ��

		CloseHandle(hFileMapT); // �ر��ں˶�����

		});
}
