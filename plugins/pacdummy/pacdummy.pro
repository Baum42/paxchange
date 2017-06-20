#-------------------------------------------------
#
# Project created by QtCreator 2017-06-10T22:15:35
#
#-------------------------------------------------

QT       += core jsonserializer

TARGET = pacdummy
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$OUT_PWD/../../pacsync

DEFINES += QT_DEPRECATED_WARNINGS \
#	QT_ASCII_CAST_WARNINGS \
	SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
	pacdummyplugin.cpp \
	pacstate.cpp

HEADERS += \
	pacdummyplugin.h \
	pacstate.h
DISTFILES += pacdummy.json

unix {
	target.path = $$[QT_INSTALL_PLUGINS]/pacsync
	INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lpacsync
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lpacsync
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lpacsync

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib

RESOURCES += \
	pacdummy.qrc
