#! /bin/tcsh


set GRM=/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SALIENT/grm-refl-json/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SALIENT/DATA_refl/english_refl-xml.grm


/opt/ParSem_db/bin/linux64/xip -sentence -sentencenum -tr -grm $GRM -xmltext 2 $1 $2 $3 $4 $5 $6 $7 $8 $9 


