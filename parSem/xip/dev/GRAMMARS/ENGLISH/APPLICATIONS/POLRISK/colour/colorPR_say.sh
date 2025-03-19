#! /bin/sh

# ce programme est une variante de l'affichage classique, basé sur les topics et leurs coordonnées, mais avec une meilleure mise en valeur des événements factuels (démo : say-negotiation). 
#Il est fait pour les recherches mettant en relation topic et information (who says what about negotiations ?), et nécessite les mêmes fichiers html que l'affichage classique (demo.html)


COLORDIR=/home/arebotie/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/POLRISK/colour
PARSEDIR=/home/arebotie/bin
cat $1 | $PARSEDIR/polrisk -offset > ./temp


#variante de l'affichage 1 : avec meilleure mise en valeur des événements factuels (démo : say-negotiation)
$COLORDIR/manip.pl temp $1 all > ./$1.xip.xml
$COLORDIR/create_html-tc.perl ./$1.xip.xml > ./topic_complet.html
$COLORDIR/create_html-tf.perl ./$1.xip.xml > ./topic_factuel.html
$COLORDIR/create_html-tcf.perl ./$1.xip.xml > ./topic_complet_factuel.html
$COLORDIR/create_html-tcf.perl ./tableaux_all.xml > ./topic_complet_tableaux.html
$COLORDIR/create_html-ev.perl ./$1.xip.xml > ./evenements.html

$COLORDIR/manip.pl temp $1 information > ./$1.xip.xml
$COLORDIR/manip.pl temp $1 topic > ./$1.xip.xml



#rm temp
