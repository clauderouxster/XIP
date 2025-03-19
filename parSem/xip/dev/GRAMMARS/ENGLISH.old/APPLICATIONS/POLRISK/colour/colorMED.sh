#! /bin/sh
set -x

# colorPR.sh fichier_txt_à_analyser

POLRISKDIR=$HOME/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK
COLORDIR=$POLRISKDIR/colour
TMPDIR=$COLORDIR/tmp

rm -rf $TMPDIR
mkdir $TMPDIR

../bin/polrisk -offset -text $1  >$TMPDIR/temp

FILE=`basename $1`
$COLORDIR/manip.pl $TMPDIR/temp $1 topic $TMPDIR >$TMPDIR/$FILE.xip.xml
$COLORDIR/create_html-tc.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/topic_complet.html
$COLORDIR/create_html-tf.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/topic_factuel.html
$COLORDIR/create_html-tcf.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/topic_complet_factuel.html
$COLORDIR/create_html-tc.perl $TMPDIR/tableaux_topic.xml > $TMPDIR/topic_complet_tableaux.html

$COLORDIR/create_html-ev.perl $TMPDIR/$FILE.xip.xml > $TMPDIR/evenements.html
cp cadre-ev.html cadre-tcf.html cadre-tc.html cadre-tc-tableau.html cadre-tf.html demo.html legende_ev.html legende.html legende_tc.html legende_tf.html liens.html logoXRCE.jpg X.jpg $TMPDIR

firefox $TMPDIR/demo.html &
