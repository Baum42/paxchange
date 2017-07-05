#!/bin/sh

if [ -z "$SUDO_USER" ]; then
    /usr/bin/pacsync "$@"
else
    sudo -i -u $SUDO_USER -H /usr/bin/pacsync -f "$@"
fi
