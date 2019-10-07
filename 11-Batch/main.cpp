#include "Batch.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Batch w;
	w.show();
	return a.exec();
}

