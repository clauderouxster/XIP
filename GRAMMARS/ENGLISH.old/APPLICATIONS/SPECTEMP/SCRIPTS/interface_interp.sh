#!/bin/sh
set -x

ROOT=$HOME/parSem/xip/dev
GRMLOC=$ROOT/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/GRMFILES
GRM1=$GRMLOC/gram_temp_tannier_sql.grm
GRM2=$GRMLOC/gram_temporalExp.grm

ROOT_XIP=$ROOT/GRAMMARS
export ROOT_XIP

JAR1=/opt/ParSem/bin/linux/jxipPy.jar
#JAR2=$ROOT/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/SCRIPTS/DocumentAnalysis.jar
JAR2=/home/xtannier/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SPECTEMP/SCRIPTS/DocumentAnalysis.jar

#PYTHONHOME=/opt/STools/gnome-2.10/lib
PYTHONHOME=/opt/STools/lib/python2.3
#PYTHONHOME=/home/roux/cvsxip/python/Python-2.4.1
export PYTHONHOME

PYTHONPATH=/opt/ParSem/bin/linux:/opt/STools/gnome-2.10/lib/python2.4:/opt/STools/gnome-2.10/lib/python2.4/lib-dynload:/home/roux/cvsxip/python/Python-2.4.1/lib/python2.4/site-packages:/home/roux/cvsxip/python/Python-2.4.1/build/lib.linux-i686-2.4
export PYTHONPATH

#xips -grm $GRMLOC/gram_temp_tannier.grm -tr -f $1 $2 $3 $4 $5 $6 $7
#/home/roux/XIPCVS/parSem/xip/dev/bin/linux/

xips -grm $GRM1 -tr -f -nodisplay -text $1

java -Xmx256m -cp $JAR1 -jar $JAR2 -grm $GRM2 -text $1 --temp_interp $2 $3 $4 $5 $6 $7 

