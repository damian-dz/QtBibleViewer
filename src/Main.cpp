#include "hdr/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(a.applicationDirPath());
    //QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(closeEvent()));
    w.show();
    return a.exec();
}
