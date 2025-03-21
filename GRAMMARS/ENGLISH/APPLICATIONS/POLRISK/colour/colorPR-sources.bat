@echo on

set ROOTDIR=c:\Users\aude
set POLRISKDIR=%ROOTDIR%\xip-9.62-01\GRAMMARS\ENGLISH\APPLICATIONS\POLRISK
set COLORDIR=%POLRISKDIR%\colour
set TMPDIR=%COLORDIR%\tmp
set PERL=c:\Perl\bin\perl

call %COLORDIR%\colorPR %1

call %POLRISKDIR%\bin\xml_polrisk %2 -offset > %TMPDIR%\temp2

%PERL% %COLORDIR%\tableau-sources-Type.pl %TMPDIR%\temp2 topic %TMPDIR%

