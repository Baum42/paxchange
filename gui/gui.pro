TEMPLATE = app

QT       += core gui widgets

TARGET = paxchange
VERSION = $$PAXCHANGEVER

RC_ICONS += ./icons/paxchange.ico
QMAKE_TARGET_COMPANY = "Baum42"
QMAKE_TARGET_PRODUCT = $$TARGET
QMAKE_TARGET_DESCRIPTION = "Paxchange"
QMAKE_TARGET_COPYRIGHT = "Felix Barz & Mike Zeller"

ICON = ./icons/paxchange.icns
QMAKE_TARGET_BUNDLE_PREFIX = de.baum42

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\""
DEFINES += "BUNDLE=\"\\\"$$QMAKE_TARGET_BUNDLE_PREFIX\\\"\""

HEADERS += \
	traycontrol.h \
	contentdialog.h \
	packagemodel.h \
	settingsdialog.h \
	consoleoperator.h \
	widgets/editpackageswidget.h \
	widgets/globalfilterwidget.h \
	wizard/databasewizard.h \
	wizard/dbselectionpage.h \
	wizard/dbpathpage.h \
	wizard/dbwidgetpage.h \
	widgets/filterswidget.h \
	widgets/extrafilterswidget.h \
	widgets/unclearpackageswidget.h \
	dbmergerdialog.h

SOURCES += \
		main.cpp \
	traycontrol.cpp \
	contentdialog.cpp \
	packagemodel.cpp \
	settingsdialog.cpp \
	consoleoperator.cpp \
	widgets/editpackageswidget.cpp \
	widgets/globalfilterwidget.cpp \
	wizard/databasewizard.cpp \
	wizard/dbselectionpage.cpp \
	wizard/dbpathpage.cpp \
	wizard/dbwidgetpage.cpp \
	widgets/filterswidget.cpp \
	widgets/extrafilterswidget.cpp \
	widgets/unclearpackageswidget.cpp \
	dbmergerdialog.cpp

FORMS += \
	settingsdialog.ui \
	widgets/editpackageswidget.ui \
	widgets/globalfilterwidget.ui \
	wizard/dbselectionpage.ui \
	wizard/dbpathpage.ui \
	widgets/filterswidget.ui \
	widgets/extrafilterswidget.ui \
	widgets/unclearpackageswidget.ui \
	dbmergerdialog.ui

RESOURCES += \
	paxchange_gui.qrc

DISTFILES += \
	application-x-paxchange-database.xml \
	paxchange_gui_de.ts \
	paxchange_gui_template.ts \
	paxchange.desktop

TRANSLATIONS += paxchange_gui_de.ts \
	paxchange_gui_template.ts

unix {
	target.path = $$[QT_INSTALL_BINS]

	trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
	trInstall.files = $$OUT_PWD/paxchange_gui_de.qm
	trInstall.CONFIG += no_check_exist

	INSTALLS += target trInstall
}

include(vendor/vendor.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lpaxchange
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lpaxchange
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lpaxchange

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib
