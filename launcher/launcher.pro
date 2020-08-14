#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T20:13:08
#
#-------------------------------------------------

QT       += core gui widgets network svg

TARGET = launcher
TEMPLATE = app

DESTDIR = $$PWD/bin

VERSION = 1.3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

RC_ICONS = logo.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    options.cpp \
    updater.cpp \
    runtimeerror.cpp \
    installnotice.cpp \
    installprogress.cpp \
    aria2client.cpp

HEADERS += \
    mainwindow.h \
    options.h \
    updater.h \
    runtimeerror.h \
    task.h \
    installnotice.h \
    installprogress.h \
    aria2client.h

FORMS += \
    mainwindow.ui \
    options.ui \
    installnotice.ui \
    installprogress.ui

RESOURCES += \
    resources.qrc

# For QArchive
QT += concurrent

LIBS += ../libs/QArchive/release/libQArchive.a -larchive -lnettle -lxml2
LIBS += -L../libs/aria2/release/lib -laria2


INCLUDEPATH += ../libs/QArchive ../libs/QArchive/include
INCLUDEPATH += ../libs/aria2/src/includes/ ../libs/aria2/src/


