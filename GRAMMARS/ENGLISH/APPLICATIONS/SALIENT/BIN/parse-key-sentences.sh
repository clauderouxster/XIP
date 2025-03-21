cat $1.txt | /home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/EERQI/bin/txtparse | egrep "IMPSENT|SUMMA|^SENT" > $1-irms.parse


/home/sandor/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/EERQI/bin/color.pl $1

