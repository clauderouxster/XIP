#! /bin/sh

COLORDIR=/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK/colour
PARSEDIR=/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK/bin
cat $1 | $PARSEDIR/txtparse -offset > ./temp
$COLORDIR/manip.pl temp $1 > ./$1.xip.xml
$COLORDIR/tableau.pl temp $1 > ./tableau.html
$COLORDIR/create_html-tc.perl ./$1.xip.xml > ./topic_complet.html
$COLORDIR/create_html-tf.perl ./$1.xip.xml > ./topic_factuel.html
$COLORDIR/create_html-ev.perl ./$1.xip.xml > ./evenements.html
$COLORDIR/create_html-tcf.perl ./$1.xip.xml > ./topic_complet_factuel.html

rm temp
