The files present is this directory  are used in order
to convert the result of the HMM tagger via XIP into the
Penn Tree Bank tagset.

- the files to be converted are obtained with the -tagger -a option of
XIP, e.g. :

cat MyFile | /opt/ParSem/bin/linux64/xip  -grm /home/brun/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SCOOP/GRMFILES/scoop-complete.grm -tr -f $1 $2 $3 $4 $5 $6 $7 $8 -tagger -a > MyFile.hmm

- In order to convert a .hmm file, you first apply the python file "convert_xiptags2PennTB.py" then "post-proc.py", typically : 

./convert_xiptags2PennTB.py MyFile.hmm > MyFile.temp
./post-proc.py MyFile.temp > MyFile.hmm.map
rm MyFile.temp
 
The shell script : convertHMM2PTB.sh convert the txt input file MyFile into the MyFile.hmm.map (and create the MyFile.hmm)
Usage : ./convertHMM2PTB.sh  MyFile


The Shell script" wholetagged.sh" uses XIP to tag all inptfiles present in /opt/ParSem_db/Corpora/treebank2/raw/wsj/NUM/*
and store the ouputfiles in /opt/ParSem_db/Corpora/MapHMM2treebank2/wsj/NUM/ (NUM ranging form 00 to 24)
Usage : ./wholetagged.sh

The Shell script wholemap.sh convert all the previously tagged files into PTB format and put them in /opt/ParSem_db/Corpora/MapHMM2treebank2/wsj/NUM/mapping2 (NUM ranging form 00 to 24)
Usage: ./wholemap.sh

