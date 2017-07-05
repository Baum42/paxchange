#!/bin/sh

srcDir=$(pwd)

#mime
mkdir -p ~/.local/share/mime/packages/
cd ~/.local/share/mime/packages/
sudo ln -s "$srcDir/gui/application-x-pacsync-database.xml"  && update-mime-database ~/.local/share/mime

#hook
cd /usr/share/libalpm/hooks
sudo ln -s "$srcDir/plugins/pacman/pacsync.hook"

#bin
cd /usr/bin
sudo ln -s "$srcDir/pacsync"
