TEMPLATE = lib

QT += jsonserializer
QT -= gui

TARGET = paxchange
VERSION = $$PAXCHANGEVER

DEFINES += PAXCHANGE_LIBRARY

isEmpty(PS_STD_PLG): DEFINES += PS_STD_PLG=\\\"\\\"
else: DEFINES += PS_STD_PLG=\\\"$$PS_STD_PLG\\\"

HEADERS += \
	packagemanagerplugin.h \
	pluginloader.h \
	exception.h \
	databasecontroller.h \
	packagedatabase.h \
	operationqueue.h \
	comboboxconfig.h \
	syncedsettings.h \
	changefilter.h \
	databasemerger.h \
    libpaxchange_global.h

SOURCES += \
	packagemanagerplugin.cpp \
	pluginloader.cpp \
	exception.cpp \
	databasecontroller.cpp \
	packagedatabase.cpp \
	operationqueue.cpp \
	comboboxconfig.cpp \
	syncedsettings.cpp \
	changefilter.cpp \
	databasemerger.cpp

TRANSLATIONS += paxchange_lib_de.ts \
	paxchange_lib_template.ts

DISTFILES += \
	paxchange_lib_template.ts \
	paxchange_lib_de.ts

unix {
	target.path = $$[QT_INSTALL_LIBS]

	trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
	trInstall.files = $$OUT_PWD/paxchange_lib_de.qm
	trInstall.CONFIG += no_check_exist

	INSTALLS += target trInstall
}

include(vendor/vendor.pri)
