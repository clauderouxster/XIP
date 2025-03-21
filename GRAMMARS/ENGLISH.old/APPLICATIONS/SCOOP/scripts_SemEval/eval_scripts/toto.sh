#! /bin/sh
java=/opt/ParSem_fast/tools/jdk/jdk1.7.0_10-linux-x64/bin/java
XIPDIR=/opt/ParSem/users/Roux/xip/parSem/xip/dev/xipFst/bin/sarenne
inputdir=../Corpus
outputdir=../Corpus

$XIPDIR/xip -f -tr -nodisplay -kif  /opt/ParSem_db/Corpora/SemEval14/tools/kif1.cxx -kifargs $1


python match.py $1  $outputdir/Restaurants_Train_XRCEresultV1.xml