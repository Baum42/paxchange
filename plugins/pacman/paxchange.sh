#!/bin/sh

pacInfos=$(</dev/stdin)

if [ -z "$SUDO_USER" ]; then
	/usr/bin/paxchange -f $1 $pacInfos
else
	sudo -i -u $SUDO_USER -H /usr/bin/paxchange -f $1 $pacInfos
fi
