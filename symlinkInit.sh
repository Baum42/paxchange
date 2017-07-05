#!/bin/sh

srcDir=$(pwd)

#mime
mkdir -p ~/.local/share/mime/packages/
cd ~/.local/share/mime/packages/
sudo ln -s "$srcDir/gui/application-x-pacsync-database.xml"  && update-mime-database ~/.local/share/mime

#hook
cd /usr/share/libalpm/hooks
sudo ln -s "$srcDir/plugins/pacman/pacsync.hook"

#hook script
cd /usr/share/libalpm/scripts
sudo ln -s "$srcDir/plugins/pacman/pacsync.sh"

#bin
cd /usr/bin
sudo ln -s "$srcDir/pacsync"
