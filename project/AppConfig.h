#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "TabBookChapterVerses.h"
#include <QDebug>
struct AppConfig
{
    struct _General
    {
        const QString sectionTitle = "General";
        QStringList keys;

        QByteArray window_geometry;
        QByteArray window_state;
        QString language;
        int max_recent_passages;

        _General() :
            keys(QStringList({ "window_geometry",
                               "window_state",
                               "language",
                               "max_recent_passages" }))
        {

        }
    };

    struct _ModuleData
    {
        const QString sectionTitle = "ModuleData";
        QStringList keys;

        int index;
        QStringList last_passage;
        QStringList compare_tab_last_verse;
        QStringList paths;
        QStringList removed_paths;

        _ModuleData() :
            keys(QStringList({ "paths",
                               "removed_paths",
                               "index",
                               "last_passage",
                               "compare_tab_last_verse" }))
        {

        }
    };

    struct _Fonts
    {
        const QString sectionTitle = "Fonts";
        QStringList keys;

        QString textbrowser_family;
        int textbrowser_size;

        _Fonts() :
            keys(QStringList({
                                  "textbrowser_font_family",
                                  "textbrowser_font_size"
                              }))
        {

        }
    };

    struct _Appearance
    {
        const QString sectionTitle = "Appearance";
        QStringList keys;

        QString window_style;
        bool use_background_image;
        QColor verse_highlight_color;
        int module_tab_position;
        int chart_animation;

        _Appearance() :
            keys(QStringList({
                                 "window_style",
                                 "use_background_image",
                                 "verse_highlight_color",
                                 "module_tab_position",
                                 "chart_animation"
                             }))
        {

        }
    };

    struct _Formatting
    {
        const QString sectionTitle = "Formatting";
        QStringList keys;

        bool reference_before;
        bool include_numbers;

        _Formatting() :
            keys(QStringList({
                                  "reference_before",
                                  "include_numbers"
                              }))
        {

        }
    };

    _General general;
    _ModuleData module_data;
    _Fonts fonts;
    _Appearance appearance;
    _Formatting formatting;

    AppConfig(const QString &path) :
        m_path(path)
    {

    }

    void load()
    {
        QSettings settings(m_path, QSettings::IniFormat);

        general.window_geometry = settings.value(general.keys[0], QByteArray()).toByteArray();
        general.window_state = settings.value(general.keys[1], QByteArray()).toByteArray();
        general.language = settings.value(general.keys[2]).toString();
        if (general.language.isNull() || general.language.isEmpty()) {
            if (QLocale::system().language() == QLocale::Polish) {
                general.language = "PL";
            } else if (QLocale::system().language() == QLocale::Spanish) {
                general.language = "ES";
            } else {
                general.language = "EN";
            }
        }
        general.max_recent_passages = settings.value(general.keys[3]).toInt();
        if (general.max_recent_passages < 2) {
            general.max_recent_passages = 20;
        }

        settings.beginGroup(module_data.sectionTitle);
        module_data.paths = settings.value(module_data.keys[0]).toStringList();
        if (module_data.paths.count() == 1
                && module_data.paths[0] == "") {
            module_data.paths.clear();
        }
        module_data.removed_paths = settings.value(module_data.keys[1]).toStringList();
        if (module_data.removed_paths.count() == 1
                && module_data.removed_paths[0] == "") {
            module_data.removed_paths.clear();
        }
        module_data.index = settings.value(module_data.keys[2]).toInt();
        module_data.last_passage = settings.value(module_data.keys[3]).toStringList();
        module_data.compare_tab_last_verse = settings.value(module_data.keys[4]).toStringList();
        if (module_data.compare_tab_last_verse.count() == 1
                && module_data.compare_tab_last_verse[0] == "") {
            module_data.compare_tab_last_verse.clear();
        }
        settings.endGroup();

        settings.beginGroup(fonts.sectionTitle);
        fonts.textbrowser_family = settings.value(fonts.keys[0]).toString();
        if (fonts.textbrowser_family.isNull() || fonts.textbrowser_family.isEmpty()) {
            fonts.textbrowser_family = qApp->font().family();
        }
        fonts.textbrowser_size = settings.value(fonts.keys[1]).toInt();
        if (fonts.textbrowser_size == 0) {
            fonts.textbrowser_size = 10;
        }
        settings.endGroup();

        settings.beginGroup(appearance.sectionTitle);
        appearance.window_style = settings.value(appearance.keys[0]).toString();
        appearance.use_background_image = settings.value(appearance.keys[1]).toBool();
        appearance.verse_highlight_color = settings.value(appearance.keys[2]).value<QColor>();
        if (!appearance.verse_highlight_color.isValid()) {
            appearance.verse_highlight_color = QColor(0, 255, 0, 50);
        }
        appearance.module_tab_position = settings.value(appearance.keys[3]).toInt();
        appearance.chart_animation = settings.value(appearance.keys[4]).toInt();
        settings.endGroup();

        settings.beginGroup(formatting.sectionTitle);
        formatting.reference_before = settings.value(formatting.keys[0]).toBool();
        formatting.include_numbers = settings.value(formatting.keys[1]).toBool();
        settings.endGroup();
    }

    void save()
    {
        QSettings settings(m_path, QSettings::IniFormat);

        settings.setValue(general.keys[0], general.window_geometry);
        settings.setValue(general.keys[1], general.window_state);
        settings.setValue(general.keys[2], general.language);
        settings.setValue(general.keys[3], general.max_recent_passages);

        settings.beginGroup(module_data.sectionTitle);
        settings.setValue(module_data.keys[0], module_data.paths);
        settings.setValue(module_data.keys[1], module_data.removed_paths);
        settings.setValue(module_data.keys[2], module_data.index);
        settings.setValue(module_data.keys[3], module_data.last_passage);
        settings.setValue(module_data.keys[4], module_data.compare_tab_last_verse);
        settings.endGroup();

        settings.beginGroup(fonts.sectionTitle);
        settings.setValue(fonts.keys[0], fonts.textbrowser_family);
        settings.setValue(fonts.keys[1], fonts.textbrowser_size);
        settings.endGroup();

        settings.beginGroup(appearance.sectionTitle);
        settings.setValue(appearance.keys[0], appearance.window_style);
        settings.setValue(appearance.keys[1], appearance.use_background_image);
        settings.setValue(appearance.keys[2], appearance.verse_highlight_color);
        settings.setValue(appearance.keys[3], appearance.module_tab_position);
        settings.setValue(appearance.keys[4], appearance.chart_animation);
        settings.endGroup();

        settings.beginGroup(formatting.sectionTitle);
        settings.setValue(formatting.keys[0], formatting.reference_before);
        settings.setValue(formatting.keys[1], formatting.include_numbers);
        settings.endGroup();
    }

private:
    QString m_path;
};

#endif // APPCONFIG_H
