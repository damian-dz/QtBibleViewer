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
    AbstractDb.cpp \
    AbstractTab.cpp \
    BiblesTabWidget.cpp \
    CompareVerseBrowser.cpp \
    ConversionTools.cpp \
    CrossReferenceBox.cpp \
    DatabaseService.cpp \
    DbBible.cpp \
    DbNotes.cpp \
    DbVerseData.cpp \
    DialogImport.cpp \
    FindOnPageBox.cpp \
    Formatting.cpp \
    Main.cpp \
    MainWindow.cpp \
    MainWindowNew.cpp \
    ModuleTabWidget.cpp \
    NavPanel.cpp \
    NavigationPanel.cpp \
    PassageBrowser.cpp \
    PassageBrowserNew.cpp \
    SearchEngine.cpp \
    DialogInfo.cpp \
    DialogPreferences.cpp \
    DialogStrong.cpp \
    SearchOptionsPanel.cpp \
    SearchOptionsPanelNew.cpp \
    SearchResultArea.cpp \
    SearchResultBrowser.cpp \
    SearchResultsArea.cpp \
    SearchResultsBrowser.cpp \
    Strings.cpp \
    TabBible.cpp \
    TabBibleNew.cpp \
    TabCompare.cpp \
    TabCompareNew.cpp \
    TabDictionary.cpp \
    TabNotes.cpp \
    TabNotesNew.cpp \
    TabSearch.cpp \
    TabSearchNew.cpp \
    WidgetCommonRare.cpp \
    WidgetCommonRareWords.cpp \
    WidgetWordFrequency.cpp

HEADERS += \
    AbstractDb.h \
    AbstractTab.h \
    AppConfig.h \
    BiblesTabWidget.h \
    CompareVerseBrowser.h \
    ConversionTools.h \
    CrossReferenceBox.h \
    DatabaseService.h \
    DbBible.h \
    DbNotes.h \
    DbVerseData.h \
    DialogImport.h \
    FindOnPageBox.h \
    Formatting.h \
    DialogInfo.h \
    DialogPreferences.h \
    DialogStrong.h \
    Location.h \
    MainWindow.h \
    MainWindowNew.h \
    Module.h \
    ModuleTabWidget.h \
    NavPanel.h \
    NavigationPanel.h \
    PassageBrowser.h \
    PassageBrowserNew.h \
    PassageWithNotes.h \
    PassageWithRef.h \
    SearchEngine.h \
    SearchOptions.h \
    SearchOptionsPanel.h \
    SearchOptionsPanelNew.h \
    SearchResultArea.h \
    SearchResultBrowser.h \
    SearchResultsArea.h \
    SearchResultsBrowser.h \
    Strings.h \
    TabBible.h \
    TabBibleNew.h \
    TabCompare.h \
    TabCompareNew.h \
    TabDictionary.h \
    TabNotes.h \
    TabNotesNew.h \
    TabSearch.h \
    TabSearchNew.h \
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
