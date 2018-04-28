#-------------------------------------------------
#
# Project created by QtCreator 2017-06-10T22:15:35
#
#-------------------------------------------------

QT       += core jsonserializer

TARGET = pacdummy
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$OUT_PWD/../../paxchange

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
	pacdummyplugin.h \
	pacstate.h

SOURCES += \
	pacdummyplugin.cpp \
	pacstate.cpp

DISTFILES += pacdummy.json

unix {
	target.path = $$[QT_INSTALL_PLUGINS]/paxchange
	INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lpaxchange
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lpaxchange
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lpaxchange

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib

RESOURCES += \
	pacdummy.qrc

QMAKE_EXTRA_TARGETS += lrelease
