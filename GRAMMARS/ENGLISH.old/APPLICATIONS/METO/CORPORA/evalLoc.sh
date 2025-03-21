#! /bin/sh

EVALDIR=/home/brun/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/METO/CORPORA/EVAL/
PARSEDIR=/home/brun/parSem/callscripts/
$PARSEDIR/parse_meto -xmltext $1
MYFILE=$1'evalFine_.txt'
sort $MYFILE | uniq > $1'.sorted'
cat $EVALDIR/LOC-INFO $1'.sorted' > $1'.ok'
perl $EVALDIR/eval.pl $1'.ok' $1 | tee $1'.resfine'
rm $1'.sorted' $1'.ok'





