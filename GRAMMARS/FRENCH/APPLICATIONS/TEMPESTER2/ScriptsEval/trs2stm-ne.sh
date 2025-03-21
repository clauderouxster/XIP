#!/bin/sh

## Ce script cree a partir d'un fichier .trs
## un fichier .stm-ne


Convert=$ROOT_XIP/FRENCH/APPLICATIONS/TEMPESTER2/ScriptsEval/convert-stm2xml.pl
#Convert=/home/brun/Corpora/Ester/convert-stm2xml.pl 
Modif=$ROOT_XIP/FRENCH/APPLICATIONS/TEMPESTER2/ScriptsEval/modifs_balises.pl
Deconvert=$ROOT_XIP/FRENCH/APPLICATIONS/TEMPESTER2/ScriptsEval/deconvert.pl

Xipparse=/home/brun/parSem/callscripts/ester2u
#Xipparse=/home/mehrmann/Scripts/fr-tempesterXml 


Name=`basename $1 ".trs"`
echo $Name
parsetrs $1 > $Name.stm
$Convert $Name.stm > $Name.xml
$Xipparse -xmltext  $Name.xml -sentence
#$Modif ./resXIP.xml > resXIP-modif.xml
$Deconvert ./resXIP.xml > $Name.stm-ne


