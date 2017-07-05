#!/bin/sh

pacInfos=$(</dev/stdin)

if [ -z "$SUDO_USER" ]; then
    /usr/bin/pacsync "$@"
else
    sudo -i -u $SUDO_USER -H /usr/bin/pacsync -f $1 $pacInfos
fi
