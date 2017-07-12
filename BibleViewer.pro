#-------------------------------------------------
#
# Project created by QtCreator 2017-02-02T22:40:38
#
#-------------------------------------------------

QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtBibleViewer
TEMPLATE = app


SOURCES += \
    src/AuxiliaryMethods.cpp \
    src/CrossReferencePopup.cpp \
    src/HistogramForm.cpp \
    src/Languages.cpp \
    src/Main.cpp \
    src/MainWindow.cpp \
    src/PreferenceDialog.cpp \
    src/SearchEngine.cpp \
    src/StrongPopup.cpp \
    src/Tab0Bible.cpp \
    src/Tab1Details.cpp \
    src/Tab2Search.cpp \
    src/Tab3Compare.cpp \
    src/Tab4Favorites.cpp \
    src/Tab5Dictionary.cpp \
    src/Tab6Topics.cpp


HEADERS  += \
    hdr/AuxiliaryMethods.h \
    hdr/CrossReferencePopup.h \
    hdr/HistogramForm.h \
    hdr/MainWindow.h \
    hdr/PreferenceDialog.h \
    hdr/SearchEngine.h \
    hdr/StrongPopup.h

FORMS    += \
    ui/CrossReferencePopup.ui \
    ui/HistogramForm.ui \
    ui/MainWindow.ui \
    ui/PreferenceDialog.ui \
    ui/StrongPopup.ui

win32: RC_ICONS = main.ico

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS_RELEASE += -O3
