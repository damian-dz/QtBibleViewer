#include "hdr/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(a.applicationDirPath());
    w.show();
    return a.exec();
}
