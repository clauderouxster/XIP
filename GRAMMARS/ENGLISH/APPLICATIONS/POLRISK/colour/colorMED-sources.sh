#!/bin/sh
set -x

POLRISKDIR=$HOME/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK
COLORDIR=$POLRISKDIR/colour
TMPDIR=$COLORDIR/tmp

$COLORDIR/colorMED.sh $1

$POLRISKDIR/bin/xml_polrisk $2 -offset > $TMPDIR/temp2

$COLORDIR/tableau-sources-Type.pl $TMPDIR/temp2 topic $TMPDIR

firefox $TMPDIR/demo.html &
