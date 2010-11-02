#include <QtGui/QApplication>
#include "qokvedmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QOkvedMainWindow w;
    w.show();

    return a.exec();
}
