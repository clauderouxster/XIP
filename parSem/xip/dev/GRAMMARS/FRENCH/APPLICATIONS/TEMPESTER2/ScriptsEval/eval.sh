#! /bin/sh

Name=`basename $1 ".trs"`

$ROOT_XIP/FRENCH/APPLICATIONS/TEMPESTER2/ScriptsEval/trs2stm-ne.sh $1 | tee resultxip.txt
stm2ctm.pl $Name.stm-ne | tee $Name.ctm-ne-test
$ROOT_XIP/FRENCH/APPLICATIONS/TEMPESTER2/ScriptsEval/transform.pl  $Name.ctm-ne-test | tee  $Name.ctm-ne
score-EN 

