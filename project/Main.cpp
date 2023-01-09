#include "MainWindow.h"
#include "MainWindowNew.h"
//#include "AppConfig.h"

//#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString appDir = app.applicationDirPath();
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString configPath = appDir + "/Data/Config/settings.ini";
    if (!QFileInfo::exists(configPath)) {
        bool settingsFound = false;
        for (const QString &location : locations) {
            if (QFileInfo::exists(location + "/settings.ini")) {
                configPath = location + "/settings.ini";
                settingsFound = true;
                break;
            }
        }
        if (!settingsFound && locations.count() > 0) {
            configPath = locations[0] + "/settings.ini";
        }
    } else {
        for (const QString &location : locations) {
            if (QFileInfo::exists(location + "/settings.ini")) {
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
        if (appTranslator.load(config.general.language.toLower(), appDir + "/Data/Lang"))
            app.installTranslator(&appTranslator);
        if (qtTranslator.load("qt_" + config.general.language.toLower(), appDir + "/Data/Lang"))
            app.installTranslator(&qtTranslator);
    }
    MainWindowNew win(appDir, config, appTranslator, qtTranslator, nullptr);
    win.show();
    return app.exec();
}
