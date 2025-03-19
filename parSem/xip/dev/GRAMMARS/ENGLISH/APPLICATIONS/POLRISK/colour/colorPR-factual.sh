#! /bin/sh
set -x

# colorPR.sh fichier_txt_à_analyser

# ce programme crée deux affichages:
# 1. un ensemble de fichiers appelés par demo.html (affichage qui privilégie les topics et affiche leurs coordonnées)
# 2. un ensemble de fichiers appelés par demo_factual.html (affichage qui privilégie les événements factuels et affiche leurs coordonnées)
#certains des fichiers créés par ce programme sont communs aux deux affichages, d'autres ne servent qu'à l'un ou à l'autre

POLRISKDIR=$HOME/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK
COLORDIR=$POLRISKDIR/colour
TMPDIR=$COLORDIR/tmp

rm -rf $TMPDIR
mkdir $TMPDIR

../bin/polrisk -offset -text $1  >$TMPDIR/temp

FILE=`basename $1`

$COLORDIR/manip.pl $TMPDIR/temp $1 factual $TMPDIR > $TMPDIR/$FILE.xip.xml
$COLORDIR/create_html-ev.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/evenements.html
$COLORDIR/create_html-fc.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/factual_complet.html
$COLORDIR/create_html-fc.perl $TMPDIR/tableaux_factual.xml > $TMPDIR/factual_complet_tableaux.html

cp cadre-ev.html cadre-fc.html cadre-tf.html cadre-fc-tableau.html demo-factual.html legende_ev.html legende.html legende_tc.html legende_tf.html liens-factual.html logoXRCE.jpg X.jpg $TMPDIR

