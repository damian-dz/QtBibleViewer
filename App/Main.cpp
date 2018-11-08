#include "MainWindow.h"

#include <QApplication>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QString appDir = app.applicationDirPath();
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString configFilePath = appDir + "/config/settings.ini";
    if (!QFileInfo(configFilePath).exists()) {
        bool settingsFound = false;
        for (QString location : locations) {
            if (QFileInfo(location + "/settings.ini").exists()) {
                configFilePath = location + "/settings.ini";
                settingsFound = true;
                break;
            }
        }
        if (!settingsFound) {
            configFilePath = locations[0] + "/settings.ini";
        }
    } else {
        for (QString location : locations) {
            if (QFileInfo(location + "/settings.ini").exists()) {
                QFile file(location + "/settings.ini");
                file.remove();
                if (location.contains("QtBibleViewer")) {
                    QDir dir(location);
                    dir.removeRecursively();
                }
            }
        }
    }
    QSettings settings(configFilePath, QSettings::IniFormat);
    QString setLanguage = settings.value(SET_LANGUAGE).toString();
    if (setLanguage.isNull() || setLanguage.isEmpty()) {
        if (QLocale::system().language() == QLocale::Polish) {
            setLanguage = "PL";
        } else if (QLocale::system().language() == QLocale::Spanish) {
            setLanguage = "ES";
        } else {
            setLanguage = "EN";
        }
    }
    QString style = settings.value(SET_STYLE).toString();
    app.setStyle(QStyleFactory::create(style));
    QTranslator translator;
    if (setLanguage != "EN") {
        translator.load(setLanguage.toLower(), appDir + "/translations");
        app.installTranslator(&translator);
    }
    MainWindow win(appDir, setLanguage.toUpper(), style, configFilePath);
    win.show();
    return app.exec();
}
