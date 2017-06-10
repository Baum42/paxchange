TEMPLATE = subdirs

SUBDIRS += \
	lib \
	gui \
    plugins

gui.depends += lib
