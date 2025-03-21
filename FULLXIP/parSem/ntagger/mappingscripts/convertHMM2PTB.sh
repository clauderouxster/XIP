#! /bin/sh
# This shell scripts tags a file with XIP HMM tagger and converts it
# into the Penn TreeBank tag set
# N.B.: Change MyDir, MyXipDir, and MyGrmDir to appropriate directories 
# USAGE : ./convertHMM2PTB.sh
MyDir=/home/brun/parSem/ntagger/mappingscripts
MyXipDir=/opt/ParSem/bin/linux64/
MyGrmDir=/home/brun/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SCOOP/GRMFILES

file=$1
$MyXipDir/xip  -grm $MyGrmDir/scoop-complete.grm -tr -f  -tagger -a < $file > $file.hmm
$MyDir/convert_xiptags2PennTB.py $file.hmm > $file.temp
$MyDir/post-proc.py $file.temp > $file.hmm.map
rm $file.temp
