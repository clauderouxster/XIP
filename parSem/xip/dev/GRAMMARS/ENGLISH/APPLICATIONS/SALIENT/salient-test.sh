#!/bin/sh
export EHOME=/var/www/html/CmdLineService/App_Data/salient-cic


$EHOME/bin/xip -sentence -sentencenum -tr -grm ./grm-domain/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SALIENT/DATA_domain/english_eerqi-xml-awapar-domain.grm -xmltext 2 $1 $2 $3 $4 $5 $6 $7 $8 $9 | egrep "^TITLE|<title>|<abstract>|<body>|^IMP|^SUM|^CONCEPT_|^SENT|^ORG|^LOCATION|^LOCORG|^PERSON|^AUTHOR|^REFLSENT|> "


xip -f -nodisplay -debug -Kif DATA_domain/jsonoutput.kif $1 $2 $3 $4 $5 $6 $7 $8 $9
g
