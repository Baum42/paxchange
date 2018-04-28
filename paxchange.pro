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

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
