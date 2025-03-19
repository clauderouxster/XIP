#!/bin/sh
export EHOME=/var/www/html/CmdLineService/App_Data/salient-cic

#$EHOME/bin/lexicon-rule.sh

#set GRM=$EHOME/grm/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/EERQI/data/english_eerqi-xml.grm

$EHOME/bin/xip -sentence -sentencenum -tr -grm ./grm/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/EERQI/data/english_eerqi-xml.grm -xmltext 2 $1 $2 $3 $4 $5 $6 $7 $8 $9 | egrep "^TITLE|<title>|<abstract>|<body>|^IMP|^SUM|^CONCEPT_|^SENT|REFERENCE|^ORG|^LOC|^PERSON|^AUTHOR|^DATE|> "
