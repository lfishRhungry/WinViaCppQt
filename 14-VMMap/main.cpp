#include "VMMap.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VMMap w;
	w.show();
	return a.exec();
}
