#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QString appDir = a.applicationDirPath();

    QString configFilePath = appDir + "/config/settings.ini";
   // if (QFileInfo(configFilePath).exists()) {
        QSettings settings(configFilePath, QSettings::IniFormat);
        QString setLanguage = settings.value("language").toString();
        if (setLanguage.isEmpty())
            setLanguage = "EN";
        QTranslator translator;
        if (setLanguage != "EN") {
            translator.load(setLanguage.toLower(), appDir + "/translations");
            a.installTranslator(&translator);
        }
   // }

    MainWindow w(appDir, setLanguage.toUpper());
    w.show();

    return a.exec();
}
