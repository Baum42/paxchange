TEMPLATE = lib

QT       -= gui

TARGET = pacsync
VERSION = $$PACSYNCVER

DEFINES += PACSYNC_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
		packagemanagerplugin.h \
		lib_global.h

SOURCES += \
		packagemanagerplugin.cpp

unix {
	target.path = /usr/lib
	INSTALLS += target
}
