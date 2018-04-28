TEMPLATE = subdirs

isEmpty(PAXCHANGE_PLUGINS): PAXCHANGE_PLUGINS = dummy pacman
contains(PAXCHANGE_PLUGINS, dummy): SUBDIRS += pacdummy
contains(PAXCHANGE_PLUGINS, pacman): SUBDIRS += pacman

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
