TEMPLATE = subdirs

SUBDIRS += \
	lib \
	gui \
	plugins

gui.depends += lib
plugins.depends += lib

qpmlupdate.target = lupdate
qpmlupdate.CONFIG += recursive
qpmlupdate.recurse_target = lupdate

qpmlrelease.target = lrelease
qpmlrelease.CONFIG += recursive
qpmlrelease.recurse_target = lrelease

QMAKE_EXTRA_TARGETS += qpmlupdate qpmlrelease
