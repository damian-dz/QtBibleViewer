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
    QString configFilePath = appDir + "/App/config/settings.ini";
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
    settings.beginGroup(GROUP_APPEARANCE);
    const QString style = settings.value(SET_STYLE).toString();
    settings.endGroup();
    app.setStyle(QStyleFactory::create(style));
    QTranslator appTranslator;
    QTranslator qtTranslator;
    if (setLanguage != "EN") {
        appTranslator.load(setLanguage.toLower(), appDir + "/App/lang");
        app.installTranslator(&appTranslator);
        qtTranslator.load("qt_" + setLanguage.toLower(), appDir + "/App/lang");
        app.installTranslator(&qtTranslator);
    }
    MainWindow win(appDir, setLanguage.toUpper(), appTranslator, qtTranslator,  style, configFilePath);
    win.show();
    return app.exec();
}
