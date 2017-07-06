TEMPLATE = lib

QT += jsonserializer
QT -= gui

TARGET = pacsync
VERSION = $$PACSYNCVER

DEFINES += PACSYNC_LIBRARY

isEmpty(PS_STD_PLG): DEFINES += PS_STD_PLG=\\\"\\\"
else: DEFINES += PS_STD_PLG=\\\"$$PS_STD_PLG\\\"

HEADERS += \
	packagemanagerplugin.h \
	pluginloader.h \
	exception.h \
	databasecontroller.h \
	packagedatabase.h \
	operationqueue.h \
	libpacsync_global.h \
	comboboxconfig.h \
	syncedsettings.h \
    changefilter.h

SOURCES += \
	packagemanagerplugin.cpp \
	pluginloader.cpp \
	exception.cpp \
	databasecontroller.cpp \
	packagedatabase.cpp \
	operationqueue.cpp \
	comboboxconfig.cpp \
	syncedsettings.cpp \
    changefilter.cpp

TRANSLATIONS += pacsync_lib_de.ts \
	pacsync_lib_template.ts

unix {
	target.path = $$[QT_INSTALL_LIBS]
	INSTALLS += target
}

include(vendor/vendor.pri)

DISTFILES += \
    pacsync_lib_template.ts \
    pacsync_lib_de.ts
