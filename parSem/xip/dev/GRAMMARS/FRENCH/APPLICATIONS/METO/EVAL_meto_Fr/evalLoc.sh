#! /bin/sh

EVALDIR=/home/llongo/parSem/xip/dev/GRAMMARS/FRENCH/APPLICATIONS/METO/EVAL_meto_Fr
PARSEDIR=/home/llongo/bin
$PARSEDIR/parse_meto_Fr -xmltext $1
MYFILE='evalFine_'$1'.txt'
sort $MYFILE | uniq > $1'.sorted'
cat $EVALDIR/LOC-INFO $1'.sorted' > $1'.ok'
perl $EVALDIR/evalFr.pl $1'.ok' $1 | tee $1'.resfine'
rm $1'.sorted' $1'.ok'





