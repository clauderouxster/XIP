@echo on

set ROOTDIR=c:\Users\aude
set POLRISKDIR=%ROOTDIR%\xip-9.62-01\GRAMMARS\ENGLISH\APPLICATIONS\POLRISK
set COLORDIR=%POLRISKDIR%\colour
set TMPDIR=%COLORDIR%\tmp
set PERL=c:\Perl\bin\perl

call %COLORDIR%%\colorPR %1

call %POLRISKDIR%\bin\graphes-xml_polrisk %2 > %TMPDIR%\sortie-graphes

%PERL% %COLORDIR%\nettoie-graphes.pl %TMPDIR%\sortie-graphes > %TMPDIR%\graphes.txt
