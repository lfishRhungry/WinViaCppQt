#include "Summation.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Summation w;
	w.show();
	return a.exec();
}
