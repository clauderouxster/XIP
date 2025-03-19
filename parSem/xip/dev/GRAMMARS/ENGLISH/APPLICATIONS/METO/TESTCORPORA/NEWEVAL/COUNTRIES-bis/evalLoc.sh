#! /bin/sh

if [ $# -ne 8 ]; then
echo "Erreur $0 a besoin d'un fichier en arg: 1/fichier test à parser 2/fichier goldstandard  3/nom fichier pour evalCoarse 3/nom fichier pour evalMedium 3/nom fichier pour evalFine"
fi

#$1 = country_toparse.test
#$2 = countries.test

#$3 = evalCoarse_country_toparse.test.text
#$4 = evalMedium_country_toparse.test.text
#$5 = evalFine_country_toparse.test.text

#$6 = XRCE-M.location.coarse
#$7 = XRCE-M.location.medium
#$8 = XRCE-M.location.fine


EVALDIR=/home/mehrmann/SemEval07/EVALwoLEX/EVAL/
INFODIR=/home/mehrmann/SemEval07/EVALwoLEX/EVAL/COUNTRIES-bis
PARSEDIR=/home/mehrmann/Scripts/
TESTDIR=/home/mehrmann/SemEval07/TESTGOLD/testgold
RESDIR=/home/mehrmann/SemEval07/EVALwoLEX/EVAL/COUNTRIES-bis/XIPLEXres
echo "FICHIER à parser = $1"
echo "FICHIER gold = $2"


# on parse le texte
echo "je parse le texte"
$PARSEDIR/en-metoXml -xmltext $1 -nodisplay

#EVAL COARSE $3
echo "je fais l evaluation coarse avec le fichier $3"
sort $3 | uniq > $RESDIR/$3'.sorted'
cat $INFODIR/LOC-INFO-coarse $RESDIR/$3'.sorted' > $RESDIR/$6
perl $EVALDIR/eval.pl $RESDIR/$6 $TESTDIR/$2 > $RESDIR/$6'.resultats'
#rm $1'.sorted' $1'.ok'



#EVAL MEDIUM $4
echo "je fais l evaluation medium avec le fichier $4"
sort $4 | uniq > $RESDIR/$4'.sorted'
cat $INFODIR/LOC-INFO-medium $RESDIR/$4'.sorted' > $RESDIR/$7
perl $EVALDIR/eval.pl $RESDIR/$7 $TESTDIR/$2 > $RESDIR/$7'.resultats'



#EVAL FINE $5
echo "je fais l evaluation medium avec le fichier $5"
sort $5 | uniq > $RESDIR/$5'.sorted'
cat $INFODIR/LOC-INFO-fine $RESDIR/$5'.sorted' > $RESDIR/$8
perl $EVALDIR/eval.pl $RESDIR/$8 $TESTDIR/$2 > $RESDIR/$8'.resultats'
