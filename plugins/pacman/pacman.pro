TEMPLATE = lib

QT += core
CONFIG += plugin

TARGET = pacman

DESTDIR = $$OUT_PWD/../../paxchange

HEADERS += \
	pacmanplugin.h

SOURCES += \
	pacmanplugin.cpp

TRANSLATIONS += paxchange_plugin_pacman_de.ts \
	paxchange_plugin_pacman_template.ts

DISTFILES += pacman.json \
	paxchange_install.hook \
	paxchange_remove.hook \
	$$TRANSLATIONS

target.path = $$[QT_INSTALL_PLUGINS]/paxchange
qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
qpmx_ts_target.files += paxchange_plugin_pacman_template.ts
hook.path = /usr/share/libalpm/hooks
hook.files += paxchange_install.hook paxchange_remove.hook
INSTALLS += target qpmx_ts_target hook

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lpaxchange
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lpaxchange
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lpaxchange

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
