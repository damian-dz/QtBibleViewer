#include "hdr/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString appDir = a.applicationDirPath();
    QSettings settings(appDir + "/config/settings.ini", QSettings::IniFormat);
    QString setLanguage = settings.value("language").toString();
    if (setLanguage.isEmpty())
        setLanguage = "ENG";
    QTranslator translator;
    if (setLanguage != "ENG") {
        translator.load(setLanguage.toLower(), appDir + "/translations");
        a.installTranslator(&translator);
    }
    MainWindow w(appDir, setLanguage, translator);
    w.show();
    return a.exec();
}
