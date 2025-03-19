#!/bin/sh
set -x

# colorPR.sh fichier_txt_à_analyser fichier_xml correspondant

POLRISKDIR=$HOME/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK
COLORDIR=$POLRISKDIR/colour
TMPDIR=$COLORDIR/tmp

rm -rf $TMPDIR
mkdir $TMPDIR

../bin/polrisk -offset -text $1  >$TMPDIR/temp

#1e analyse : pour le coloriage
FILE=`basename $1`
$COLORDIR/manip.pl $TMPDIR/temp $1 topic $TMPDIR >$TMPDIR/$FILE.xip.xml
$COLORDIR/create_html-tc.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/topic_complet.html
$COLORDIR/create_html-tf.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/topic_factuel.html
$COLORDIR/create_html-tc.perl $TMPDIR/tableaux_topic.xml > $TMPDIR/topic_complet_tableaux.html
$COLORDIR/create_html-ev.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/evenements.html

# 2e analyse : pour les graphes xml
$POLRISKDIR/bin/graphes-xml_polrisk $2 > $TMPDIR/sortie-graphes
$COLORDIR/nettoie-graphes.pl $TMPDIR/sortie-graphes > $TMPDIR/graphes.txt

# 3e analyse : pour le tableau
$POLRISKDIR/bin/xml_polrisk $2 -offset > $TMPDIR/temp2
$COLORDIR/tableau-sources-Type.pl $TMPDIR/temp2 topic $TMPDIR


cp cadre-ev.html cadre-tc.html cadre-tc-tableau.html cadre-tf.html demo-graphes.html legende_ev.html legende.html legende_tc.html legende_tf.html liens-graphes.html logoXRCE.jpg X.jpg $TMPDIR

