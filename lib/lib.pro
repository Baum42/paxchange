TEMPLATE = lib

QT += jsonserializer
QT -= gui

TARGET = pacsync
VERSION = $$PACSYNCVER

DEFINES += PACSYNC_LIBRARY

!custom_standard_plg: DEFINES += PS_STD_PLG=\\\"\\\"

HEADERS += \
	packagemanagerplugin.h \
	pluginloader.h \
	exception.h \
	databasecontroller.h \
	packagedatabase.h \
	operationqueue.h \
	libpacsync_global.h \
	comboboxconfig.h \
    syncedsettings.h

SOURCES += \
	packagemanagerplugin.cpp \
	pluginloader.cpp \
	exception.cpp \
	databasecontroller.cpp \
	packagedatabase.cpp \
	operationqueue.cpp \
	comboboxconfig.cpp \
    syncedsettings.cpp

TRANSLATIONS += pacsync_lib_de.ts \
	pacsync_lib_template.ts

unix {
	target.path = $$[QT_INSTALL_LIBS]
	INSTALLS += target
}

include(vendor/vendor.pri)
