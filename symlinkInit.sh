#!/bin/sh

srcDir=$(pwd)

#mime
mkdir -p ~/.local/share/mime/packages/
cd ~/.local/share/mime/packages/
sudo ln -s "$srcDir/gui/application-x-paxchange-database.xml"  && update-mime-database ~/.local/share/mime

#hooks
cd /usr/share/libalpm/hooks
sudo ln -s "$srcDir/plugins/pacman/paxchange_install.hook"

cd /usr/share/libalpm/hooks
sudo ln -s "$srcDir/plugins/pacman/paxchange_remove.hook"

#hook script
cd /usr/share/libalpm/scripts
sudo ln -s "$srcDir/plugins/pacman/paxchange.sh"

#bin
cd /usr/bin
sudo ln -s "$srcDir/paxchange"
