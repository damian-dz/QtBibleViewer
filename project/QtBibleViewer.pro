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

PRECOMPILED_HEADER = precomp.h

SOURCES += \
    AbstractTab.cpp \
    BibleNavigationPanel.cpp \
    BiblePassageBrowser.cpp \
    Formatting.cpp \
    Main.cpp \
    ModuleTabWidget.cpp \
    NewMainWindow.cpp \
    SearchEngine.cpp \
    DialogPreferences.cpp \
    DialogStrong.cpp \
    DialogXRefs.cpp \
    SearchOptionsPanel.cpp \
    SearchResultsArea.cpp \
    TabBible.cpp \
    TabCompare.cpp \
    TabSearch.cpp \
    WidgetCommonWords.cpp \
    WidgetHistogram.cpp \
    WidgetCommonRare.cpp \
    DialogInfo.cpp

HEADERS += \
    AbstractTab.h \
    BibleNavigationPanel.h \
    BiblePassageBrowser.h \
    Formatting.h \
    Location.h \
    DialogPreferences.h \
    DialogStrong.h \
    DialogXRefs.h \
    ModuleData.h \
    ModuleTabWidget.h \
    NewMainWindow.h \
    SearchEngine.h \
    SearchOptions.h \
    SearchOptionsPanel.h \
    SearchResultsArea.h \
    TabBible.h \
    TabCompare.h \
    TabSearch.h \
    WidgetCommonWords.h \
    WidgetHistogram.h \
    WidgetCommonRare.h \
    AppConfig.h \
    DialogInfo.h


RESOURCES += \
    res.qrc

RC_FILE = app.rc
