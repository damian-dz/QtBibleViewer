#-------------------------------------------------
#
# Project created by QtCreator 2018-01-24T11:52:22
#
#-------------------------------------------------

QT += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtBibleViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QMAKE_LFLAGS += -no-pie
CONFIG += c++17

PRECOMPILED_HEADER = precomp.h

SOURCES += \
    AbstractTab.cpp \
    CompareVerseBrowser.cpp \
    ConversionTools.cpp \
    CrossReferenceBox.cpp \
    DialogImport.cpp \
    FindOnPageBox.cpp \
    Formatting.cpp \
    Main.cpp \
    MainWindow.cpp \
    ModuleTabWidget.cpp \
    NavigationPanel.cpp \
    PassageBrowser.cpp \
    SearchEngine.cpp \
    DialogInfo.cpp \
    DialogPreferences.cpp \
    DialogStrong.cpp \
    SearchOptionsPanel.cpp \
    SearchResultArea.cpp \
    SearchResultBrowser.cpp \
    Strings.cpp \
    TabBible.cpp \
    TabCompare.cpp \
    TabDictionary.cpp \
    TabNotes.cpp \
    TabSearch.cpp \
    WidgetCommonRare.cpp \
    WidgetCommonRareWords.cpp \
    WidgetWordFrequency.cpp

HEADERS += \
    AbstractTab.h \
    AppConfig.h \
    CompareVerseBrowser.h \
    ConversionTools.h \
    CrossReferenceBox.h \
    DialogImport.h \
    FindOnPageBox.h \
    Formatting.h \
    DialogInfo.h \
    DialogPreferences.h \
    DialogStrong.h \
    Location.h \
    MainWindow.h \
    Module.h \
    ModuleTabWidget.h \
    NavigationPanel.h \
    PassageBrowser.h \
    PassageWithRef.h \
    SearchEngine.h \
    SearchOptions.h \
    SearchOptionsPanel.h \
    SearchResultArea.h \
    SearchResultBrowser.h \
    Strings.h \
    TabBible.h \
    TabCompare.h \
    TabDictionary.h \
    TabNotes.h \
    TabSearch.h \
    TabbedLocation.h \
    WidgetCommonRare.h \
    WidgetCommonRareWords.h \
    WidgetWordFrequency.h

RESOURCES += \
    res.qrc

RC_FILE = app.rc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/3rd_party/sqlite3/lib/ -lsqlite3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rd_party/sqlite3/lib/ -lsqlite3d
else:unix: LIBS += -L$$PWD/3rd_party/sqlite3/lib/ -lsqlite3

INCLUDEPATH += $$PWD/3rd_party/sqlite3/api
DEPENDPATH += $$PWD/3rd_party/sqlite3/api
