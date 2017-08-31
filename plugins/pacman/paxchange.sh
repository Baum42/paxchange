#!/bin/sh
pacInfos=$(</dev/stdin)
/usr/bin/paxchange -f $1 $pacInfos
