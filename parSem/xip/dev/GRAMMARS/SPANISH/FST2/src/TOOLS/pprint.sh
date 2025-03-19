#!/bin/sh
#

TOOLS=$HOME/parSem/xip/dev/GRAMMARS/SPANISH/FST2/src/TOOLS

gawk -f $TOOLS/pprint.awk $* | sort -d

