#-------------------------------------------------
#
# Project created by QtCreator 2017-06-21T18:56:29
#
#-------------------------------------------------

QT       += core

TARGET = pacman
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$OUT_PWD/../../pacsync

HEADERS += \
	pacmanplugin.h

SOURCES += \
	pacmanplugin.cpp

DISTFILES += pacman.json \
	pacsync_plugin_pacman_de.ts \
	pacsync_plugin_pacman_template.ts

TRANSLATIONS += pacsync_plugin_pacman_de.ts \
	pacsync_plugin_pacman_template.ts

unix {
	target.path = $$[QT_INSTALL_PLUGINS]/pacsync

	trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
	trInstall.files = $$OUT_PWD/pacsync_plugin_pacman_de.qm
	trInstall.CONFIG += no_check_exist

	INSTALLS += target trInstall
}

include(vendor/vendor.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lpacsync
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lpacsync
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lpacsync

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib
