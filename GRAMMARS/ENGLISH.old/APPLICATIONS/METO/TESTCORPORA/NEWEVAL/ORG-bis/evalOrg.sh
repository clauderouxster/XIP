	#! /bin/sh

#EVALDIR=/home/brun/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/METO/CORPORA/EVAL/
#PARSEDIR=/home/brun/parSem/callscripts/
#$PARSEDIR/parse_meto -xmltext $1
#MYFILE='evalFine_'$1'.txt'
#sort $MYFILE | uniq > $1'.sorted'
#cat $EVALDIR/ORG-INFO $1'.sorted' > $1'.ok'
#perl $EVALDIR/eval.pl $1'.ok' $1 | tee $1'.resfine'
#rm $1'.sorted'


#! /bin/sh

if [ $# -ne 8 ]; then
echo "Erreur $0 a besoin d'un fichier en arg: 1/fichier test à parser 2/fichier goldstandard  3/nom fichier pour evalCoarse 3/nom fichier pour evalMedium 3/nom fichier pour evalFine"
fi

#$1 = comp_toparse.test
#$2 = companies.test

#$3 = evalCoarse_comp_toparse.test.text
#$4 = evalMedium_comp_toparse.test.text
#$5 = evalFine_comp_toparse.test.text

#$6 = XRCE-M.org.coarse
#$7 = XRCE-M.org.medium
#$8 = XRCE-M.org.fine


EVALDIR=/home/mehrmann/SemEval07/EVALwoLEX/EVAL/
INFODIR=/home/mehrmann/SemEval07/EVALwoLEX/EVAL/ORG-bis
PARSEDIR=/home/mehrmann/Scripts/
TESTDIR=/home/mehrmann/SemEval07/TESTGOLD/testgold
RESDIR=/home/mehrmann/SemEval07/EVALwoLEX/EVAL/ORG-bis/XIPLEXres
echo "FICHIER à parser = $1"
echo "FICHIER gold = $2"


# on parse le texte
echo "je parse le texte"
$PARSEDIR/en-metoXml -xmltext $1 -nodisplay

#EVAL COARSE $3
echo "je fais l evaluation coarse avec le fichier $3"
sort $3 | uniq > $RESDIR/$3'.sorted'
cat $INFODIR/ORG-INFO-coarse $RESDIR/$3'.sorted' > $RESDIR/$6
perl $EVALDIR/eval.pl $RESDIR/$6 $TESTDIR/$2 > $RESDIR/$6'.resultats'
#rm $1'.sorted' $1'.ok'



#EVAL MEDIUM $4
echo "je fais l evaluation medium avec le fichier $4"
sort $4 | uniq > $RESDIR/$4'.sorted'
cat $INFODIR/ORG-INFO-medium $RESDIR/$4'.sorted' > $RESDIR/$7
perl $EVALDIR/eval.pl $RESDIR/$7 $TESTDIR/$2 > $RESDIR/$7'.resultats'



#EVAL FINE $5
echo "je fais l evaluation medium avec le fichier $5"
sort $5 | uniq > $RESDIR/$5'.sorted'
cat $INFODIR/ORG-INFO-fine $RESDIR/$5'.sorted' > $RESDIR/$8
perl $EVALDIR/eval.pl $RESDIR/$8 $TESTDIR/$2 > $RESDIR/$8'.resultats'



