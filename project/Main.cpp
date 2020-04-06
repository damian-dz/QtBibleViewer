#include "MainWindow.h"
//#include "AppConfig.h"

//#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    QString appDir = app.applicationDirPath();
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString configPath = appDir + "/App/config/settings.ini";
    if (!QFileInfo(configPath).exists()) {
        bool settingsFound = false;
        for (QString location : locations) {
            if (QFileInfo(location + "/settings.ini").exists()) {
                configPath = location + "/settings.ini";
                settingsFound = true;
                break;
            }
        }
        if (!settingsFound) {
            configPath = locations[0] + "/settings.ini";
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
    AppConfig config(configPath);
    config.load();
    app.setStyle(QStyleFactory::create(config.appearance.window_style));
    QTranslator appTranslator;
    QTranslator qtTranslator;
    if (config.general.language != "EN") {
        appTranslator.load(config.general.language.toLower(), appDir + "/App/lang");
        app.installTranslator(&appTranslator);
        qtTranslator.load("qt_" + config.general.language.toLower(), appDir + "/App/lang");
        app.installTranslator(&qtTranslator);
    }
    MainWindow win(appDir, config, appTranslator, qtTranslator, nullptr);
    win.show();
    return app.exec();
}
