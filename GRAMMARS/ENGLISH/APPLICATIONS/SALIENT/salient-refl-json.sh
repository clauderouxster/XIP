#!/bin/sh

export EHOME=/var/www/html/CmdLineService/App_Data/salient-cic


$EHOME/bin-kif/xip -f -tr -Kif ./grm-refl-json/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SALIENT/DATA_refl_json/jsonoutput.kif $1 $2 $3 $4 $5 $6 $7 $8 $9
