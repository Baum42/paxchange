#-------------------------------------------------
#
# Project created by QtCreator 2017-06-21T18:56:29
#
#-------------------------------------------------

QT       += core

TARGET = pacman
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$OUT_PWD/../../paxchange

HEADERS += \
	pacmanplugin.h

SOURCES += \
	pacmanplugin.cpp

DISTFILES += pacman.json \
    paxchange.sh \
    paxchange_install.hook \
    paxchange_plugin_pacman_de.ts \
    paxchange_plugin_pacman_template.ts \
    paxchange_remove.hook

TRANSLATIONS += paxchange_plugin_pacman_de.ts \
	paxchange_plugin_pacman_template.ts

unix {
	target.path = $$[QT_INSTALL_PLUGINS]/paxchange

	trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
	trInstall.files = $$OUT_PWD/paxchange_plugin_pacman_de.qm
	trInstall.CONFIG += no_check_exist

	INSTALLS += target trInstall
}

include(vendor/vendor.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lpaxchange
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lpaxchange
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lpaxchange

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib
