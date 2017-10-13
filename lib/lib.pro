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
	paxchange.pc \
	$$TRANSLATIONS

unix {
	target.path = $$[QT_INSTALL_LIBS]

	qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
	qpmx_ts_target.files += paxchange_lib_template.ts

	hdrInstall.path = /usr/include/$$TARGET
	hdrInstall.files = $$HEADERS

	pkgInstall.path = /usr/lib/pkgconfig
	pkgInstall.files = paxchange.pc

	INSTALLS += target qpmx_ts_target hdrInstall pkgInstall
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
