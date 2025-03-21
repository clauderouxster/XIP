cat $1.txt | /home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/ELSEVIER/bin/txtparse | egrep "> |^SUMMARY|^IMP|^LOGIC" > $1.parse
/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/ELSEVIER/bin/color.pl $1



