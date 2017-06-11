TEMPLATE = lib

QT       -= gui

TARGET = pacsync
VERSION = $$PACSYNCVER

DEFINES += PACSYNC_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

!custom_standard_plg: DEFINES += PS_STD_PLG=\\\"generic/\\\"

HEADERS += \
	packagemanagerplugin.h \
	lib_global.h \
	pluginloader.h \
	exception.h \
    databasecontroller.h \
    packagedatabase.h

SOURCES += \
	packagemanagerplugin.cpp \
	pluginloader.cpp \
	exception.cpp \
    databasecontroller.cpp \
    packagedatabase.cpp

unix {
	target.path = $$[QT_INSTALL_LIBS]
	INSTALLS += target
}
