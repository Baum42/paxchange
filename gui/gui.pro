TEMPLATE = app

QT       += core gui widgets

TARGET = pacsync
VERSION = $$PACSYNCVER

RC_ICONS += ./icons/pacsync.ico
QMAKE_TARGET_COMPANY = "Baum42"
QMAKE_TARGET_PRODUCT = $$TARGET
QMAKE_TARGET_DESCRIPTION = "Pac-Sync"
QMAKE_TARGET_COPYRIGHT = "Felix Barz & Mike Zeller"

ICON = ./icons/pacsync.icns
QMAKE_TARGET_BUNDLE_PREFIX = de.baum42

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\""
DEFINES += "BUNDLE=\"\\\"$$QMAKE_TARGET_BUNDLE_PREFIX\\\"\""

HEADERS += \
	traycontrol.h \
	contentdialog.h \
	editpackageswidget.h \
	databasewizard.h \
	dbselectionpage.h \
	dbpathpage.h \
	dbpackagespage.h \
	packagemodel.h \
	settingsdialog.h \
	consoleoperator.h \
    globalfilterwidget.h

SOURCES += \
		main.cpp \
	traycontrol.cpp \
	contentdialog.cpp \
	editpackageswidget.cpp \
	databasewizard.cpp \
	dbselectionpage.cpp \
	dbpathpage.cpp \
	dbpackagespage.cpp \
	packagemodel.cpp \
	settingsdialog.cpp \
	consoleoperator.cpp \
    globalfilterwidget.cpp

FORMS += \
	editpackageswidget.ui \
	dbselectionpage.ui \
	dbpathpage.ui \
	settingsdialog.ui \
    globalfilterwidget.ui

RESOURCES += \
	pacsync_gui.qrc

DISTFILES += \
	application-x-pacsync-database.xml

TRANSLATIONS += pacsync_gui_de.ts \
	pacsync_gui_template.ts

unix {
	target.path = $$[QT_INSTALL_BINS]
	INSTALLS += target
}

include(vendor/vendor.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lpacsync
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lpacsync
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lpacsync

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib
