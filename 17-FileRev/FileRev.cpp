#include "FileRev.h"

// �����ļ�����
#define FILENAME  TEXT("FileRev.dat")

FileRev::FileRev(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// -----------------------�������-------------------------
	this->setFixedSize(800, 200);

	ui.labelPath->setText("Pathname:");

	ui.lineEdit->setReadOnly(true);

	ui.buttonBrowse->setText("Browse");

	ui.buttonReverse->setText("Reverse file contents");

	ui.buttonReverse->setEnabled(false);

	ui.labelChar->setText("Type of characters in file:");

	ui.labelType->setText("Unknown");

	ui.labelType->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	// -----------------------����������-------------------------

	// browse�ļ�·��
	connect(ui.buttonBrowse, &QPushButton::clicked, [=]() {
		ui.lineEdit->setText(
			QFileDialog::getOpenFileName(this, "Choose a txt file", "C:/Users/lfish/Desktop", "(*.txt)"));
		});

	connect(ui.lineEdit, &QLineEdit::textChanged, [=](const QString& text) {

		if (text.isEmpty()) {
			ui.buttonReverse->setEnabled(false);
		}
		else
		{
			ui.buttonReverse->setEnabled(true);
		}
		});

	// reverse���߼�
	connect(ui.buttonReverse, &QPushButton::clicked, [=]() {

		const wchar_t* wstrPathName = reinterpret_cast<const wchar_t*>(ui.lineEdit->text().utf16());
		
		// Ϊ�˽��ɹ��������ļ�ͬ��Ŀ¼ ��ȡ�ļ�����Ŀ¼ ���ı������Ŀ¼
		PTCHAR psPath[_MAX_DIR];
		_wsplitpath(wstrPathName, nullptr, (wchar_t*)psPath, nullptr, nullptr);
		SetCurrentDirectory((LPCWSTR)psPath);

		// �ȸ���һ���ļ� ʹ�ø��� �ļ������� ������Դ�ļ�
		if (!CopyFile(wstrPathName, FILENAME, FALSE)) {
			QMessageBox::critical(this, "Error", "New file could not be created.");
			return;
		}

		BOOL bIsTextUnicode;
		if (FileReverse(FILENAME, &bIsTextUnicode)) {
			// ���±�������
			ui.labelType->setText(QString::fromWCharArray(bIsTextUnicode ? TEXT("Unicode") : TEXT("ANSI")));

			// �򿪼��±��鿴���ǵĳɹ�
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			TCHAR sz[] = TEXT("Notepad ") FILENAME; // ���ú�������
			if (CreateProcess(NULL, sz,
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {

				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
		}
		});
	
}

// ��ת�ļ��ĺ���
BOOL FileRev::FileReverse(PCTSTR pszPathname, PBOOL pbIsTextUnicode) {

	*pbIsTextUnicode = FALSE;  // ����ʱUnicode

	// ���ļ�
	HANDLE hFile = CreateFile(pszPathname, GENERIC_WRITE | GENERIC_READ, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		QMessageBox::critical(this, "Error", "File could not be opened");
		return(FALSE);
	}

	// ��ȡ�ļ���С�������ļ�ʱ���Ա���ȫӳ��ģ�
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// �����ļ�ӳ����� ���С����Ϊ���ļ���һ�����ֽ� �Ա������β
	// ��Ϊ���ǲ�֪���ļ�ʱANSI����Unicode �ͷ�һ��wchar��С���ֽ�
	// ע�� ��������Ǵ�����һ���ļ�ӳ���ں˶���
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
		0, dwFileSize + sizeof(WCHAR), NULL);

	if (hFileMap == NULL) {
		QMessageBox::critical(this, "Error", "File map could not be opened.");
		CloseHandle(hFile);
		return(FALSE);
	}

	// Ϊ���ļ�ӳ���ں˶���Ԥ���������ռ� ����ȡ�׵�ַ
	PVOID pvFile = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if (pvFile == NULL) {
		QMessageBox::critical(this, "Error", "Could not map view of file.");
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return(FALSE);
	}

	// ������ANSI����Unicode
	int iUnicodeTestFlags = -1;
	*pbIsTextUnicode = IsTextUnicode(pvFile, dwFileSize, &iUnicodeTestFlags);

	if (!*pbIsTextUnicode) {
		// �ļ�ʱANSI

		// ����������
		PSTR pchANSI = (PSTR)pvFile;
		pchANSI[dwFileSize / sizeof(CHAR)] = 0;

		// �ļ���ת(�벻������������API)
		_strrev(pchANSI);

		// ����Ҫ�����б�������ת��"\n\r"�ĵط�ת��"\r\n"
		pchANSI = strstr(pchANSI, "\n\r");

		while (pchANSI != NULL) {
			*pchANSI++ = '\r';   // �ȵ��ں�Ӽ� ���˰�
			*pchANSI++ = '\n';
			pchANSI = strstr(pchANSI, "\n\r"); // ��һ��
		}

	}
	else {
		// Unicode�����

		// ��β����
		PWSTR pchUnicode = (PWSTR)pvFile;
		pchUnicode[dwFileSize / sizeof(WCHAR)] = 0;

		// �ж��Ƿ��Ǵ�BOM(byte-order-mark)��Unicode
		if ((iUnicodeTestFlags & IS_TEXT_UNICODE_SIGNATURE) != 0) {
			// �Ǿ�����
			pchUnicode++;
		}

		// ��ת
		_wcsrev(pchUnicode);

		// ������һ���Ĳ���
		pchUnicode = wcsstr(pchUnicode, L"\n\r");

		while (pchUnicode != NULL) {
			*pchUnicode++ = L'\r';
			*pchUnicode++ = L'\n';
			pchUnicode = wcsstr(pchUnicode, L"\n\r");
		}
	}

	// ��ת��� �Ը��ļ�ӳ����޸ľ��Ƕ�Դ�ļ����޸�
	// �������
	UnmapViewOfFile(pvFile);
	CloseHandle(hFileMap);

	// ����ӳ���ļ���ʱ���ӳ����һ����Ž�β���ֽڵĴ�С
	// ��������ض�
	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return(TRUE);
}
