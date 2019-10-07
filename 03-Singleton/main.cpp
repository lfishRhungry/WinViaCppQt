#include "Singleton.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Singleton w;
	w.show();
	return a.exec();
}
