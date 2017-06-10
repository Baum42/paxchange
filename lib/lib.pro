TEMPLATE = lib

QT       -= gui

TARGET = pacsync
VERSION = $$PACSYNCVER

DEFINES += PACSYNC_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
		packagemanagerplugin.h \
		lib_global.h \
    pluginloader.h \
    exception.h

SOURCES += \
		packagemanagerplugin.cpp \
    pluginloader.cpp \
    exception.cpp

unix {
	target.path = $$[QT_INSTALL_LIBS]
	INSTALLS += target
}
