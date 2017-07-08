TEMPLATE = subdirs

isEmpty(PAXCHANGE_PLUGINS): PAXCHANGE_PLUGINS = dummy pacman
contains(PAXCHANGE_PLUGINS, dummy): SUBDIRS += pacdummy
contains(PAXCHANGE_PLUGINS, pacman): SUBDIRS += pacman

qpmlupdate.target = lupdate
qpmlupdate.CONFIG += recursive
qpmlupdate.recurse_target = lupdate

qpmlrelease.target = lrelease
qpmlrelease.CONFIG += recursive
qpmlrelease.recurse_target = lrelease

QMAKE_EXTRA_TARGETS += qpmlupdate qpmlrelease
