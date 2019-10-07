#include "FileRev.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FileRev w;
	w.show();
	return a.exec();
}
