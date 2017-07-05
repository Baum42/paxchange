TEMPLATE = subdirs

isEmpty(PACSYNC_PLUGINS): PACSYNC_PLUGINS = dummy pacman
contains(PACSYNC_PLUGINS, dummy): SUBDIRS += pacdummy
contains(PACSYNC_PLUGINS, pacman): SUBDIRS += pacman

qpmlupdate.target = lupdate
qpmlupdate.CONFIG += recursive
qpmlupdate.recurse_target = lupdate

qpmlrelease.target = lrelease
qpmlrelease.CONFIG += recursive
qpmlrelease.recurse_target = lrelease

QMAKE_EXTRA_TARGETS += qpmlupdate qpmlrelease
