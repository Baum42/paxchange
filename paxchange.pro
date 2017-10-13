TEMPLATE = subdirs

SUBDIRS += \
	lib \
	gui \
	plugins

DISTFILES += README.md \
	LICENSE \
	TODO.md \
	symlinkInit.sh \
	.qmake.conf

gui.depends += lib
plugins.depends += lib

qpmxlrelease.target = lrelease
qpmxlrelease.CONFIG += recursive
qpmxlrelease.recurse_target = lrelease

QMAKE_EXTRA_TARGETS += qpmxlrelease
