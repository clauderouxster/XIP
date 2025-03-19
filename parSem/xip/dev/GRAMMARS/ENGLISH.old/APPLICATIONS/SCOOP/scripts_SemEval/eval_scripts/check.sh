#! /bin/sh
java=/opt/ParSem_fast/tools/jdk/jdk1.7.0_10-linux-x64/bin/java
XIPDIR=/opt/ParSem/users/Roux/xip/parSem/xip/dev/xipFst/bin/sarenne
inputdir=../Corpus
outputdir=../Corpus
file=/opt/ParSem/users/diana/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SCOOP/Corpus/SemEval/Restaurants/Restaurants/restaurants-trial_XRCE.xml 

# USAGE : ./check.sh training/test
# the training and trial files are in ../corpus
# the result of the kif file (Restaurants_Train_XRCEresultV1.xml)  is also in ../corpus
echo ">>>> PARSE" 

#$XIPDIR/xip -f -tr -nodisplay -kif  /opt/ParSem_db/Corpora/SemEval14/tools/kif1.cxx -kifargs /opt/ParSem_db/Corpora/SemEval14/Restaurants/Restaurants_Train_v2.xml

$XIPDIR/xip -f -tr -nodisplay -kif  kif1.cxx -kifargs $1


echo ">>>>>> ASPECTS"
java -cp ./eval.jar Main.Aspects $outputdir/Restaurants_Train_XRCEresultV1.xml $1
#echo ">>>>>>>CATEG"

java -cp ./eval.jar Main.Polarity  $outputdir/Restaurants_Train_XRCEresultV1.xml $1
