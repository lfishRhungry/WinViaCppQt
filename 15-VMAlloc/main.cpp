#include "VMAlloc.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VMAlloc w;
	w.show();
	return a.exec();
}
