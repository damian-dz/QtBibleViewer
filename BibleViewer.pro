#-------------------------------------------------
#
# Project created by QtCreator 2017-02-02T22:40:38
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BibleViewer
TEMPLATE = app


SOURCES += \
    src/CrossReferencePopup.cpp \
    src/Languages.cpp \
    src/Main.cpp \
    src/MainWindow.cpp \
    src/PreferenceDialog.cpp \
    src/StrongPopup.cpp \
    src/Tab0Bible.cpp \
    src/Tab1Details.cpp \
    src/Tab2Search.cpp \
    src/Tab3Dictionary.cpp \
    src/Tab4Topics.cpp

HEADERS  += \
    hdr/CrossReferencePopup.h \
    hdr/MainWindow.h \
    hdr/PreferenceDialog.h \
    hdr/StrongPopup.h

FORMS    += \
    ui/CrossReferencePopup.ui \
    ui/MainWindow.ui \
    ui/PreferenceDialog.ui \
    ui/StrongPopup.ui

# win32: RC_ICONS = main.ico

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS_RELEASE += -O3
