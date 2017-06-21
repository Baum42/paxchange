#-------------------------------------------------
#
# Project created by QtCreator 2017-06-21T18:56:29
#
#-------------------------------------------------

QT       += core

TARGET = pacman
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$OUT_PWD/../../pacsync

SOURCES += \
    pacman.cpp

HEADERS += \
    pacman.h
DISTFILES += pacman.json 

unix {
    target.path = $[QT_INSTALL_PLUGINS]/pacsync
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lpacsync
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lpacsync
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lpacsync

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib
