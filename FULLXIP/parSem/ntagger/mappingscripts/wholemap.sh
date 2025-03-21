#! /bin/sh

MyDir=/home/brun/parSem/ntagger/mappingscripts
OutputDir=/opt/ParSem_db/Corpora/MapHMM2treebank2/wsj/
for val in 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
do
    cd $OutputDir/$val/
    for file in wsj_*.hmm 
    do 
        $MyDir/convert_xiptags2PennTB.py $file > $OutputDir/$val/mapping2/temp
        $MyDir/post-proc.py $OutputDir/$val/mapping2/temp >  $OutputDir/$val/mapping2/$file.map
        rm $OutputDir/$val/mapping2/temp
    done
done