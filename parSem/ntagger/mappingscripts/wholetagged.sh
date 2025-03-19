#! /bin/sh

MyDir=/home/brun/parSem/ntagger/mappingscripts
MyXipDir=/opt/ParSem/bin/linux64/
MyGrmDir=/home/brun/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SCOOP/GRMFILES
OutputDir=/opt/ParSem_db/Corpora/MapHMM2treebank2/wsj/
InputDir=/opt/ParSem_db/Corpora/treebank2/raw/wsj/
for val in 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
do 
    cd $InputDir/$val/
    for file in wsj_*
    do    
        $MyDir/suppress-start.py $InputDir/$val/$file > $OutputDir/$val/temp
        $MyXipDir/xip  -grm $MyGrmDir/scoop-complete.grm -tr -f  -tagger -a < $OutputDir/$val/temp > $OutputDir/$val/$file.hmm
        rm $OutputDir/$val/temp
    done
done