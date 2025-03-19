@echo on

REM ce programme crée deux affichages:
REM 1. un ensemble de fichiers appelés par demo.html (affichage qui privilégie les topics et affiche leurs coordonnées)
REM 2. un ensemble de fichiers appelés par demo_factual.html (affichage qui privilégie les événements factuels et affiche leurs coordonnées)
REM certains des fichiers créés par ce programme sont communs aux deux affichages, d'autres ne servent qu'à l'un ou à l'autre

set ROOTDIR=c:\Users\parSem\demo-EventSpotter
set POLRISKDIR=%ROOTDIR%\xip-9.62-01\GRAMMARS\ENGLISH\APPLICATIONS\POLRISK
set COLORDIR=%POLRISKDIR%\colour
set TMPDIR=%COLORDIR%\tmp
set PERL=c:\Perl\bin\perl

del %TMPDIR%\* /Q
rmdir %TMPDIR%
mkdir %TMPDIR%

call %POLRISKDIR%\bin\polrisk.bat -offset -text %1 > %TMPDIR%\temp

REM les 5 lignes suivantes peuvent être commentées lorsqu'on ne veut que l'affichage 2 (factuel)
%PERL% %COLORDIR%\manip.pl %TMPDIR%\temp %1 topic %TMPDIR% > %TMPDIR%\%1.xip.xml
%PERL% %COLORDIR%\create_html-tc.perl %TMPDIR%\%1.xip.xml > %TMPDIR%\topic_complet.html
%PERL% %COLORDIR%\create_html-tf.perl %TMPDIR%\%1.xip.xml > %TMPDIR%\topic_factuel.html
%PERL% %COLORDIR%\create_html-tcf.perl %TMPDIR%\%1.xip.xml > %TMPDIR%\topic_complet_factuel.html
%PERL% %COLORDIR%\create_html-tc.perl %TMPDIR%\tableaux_topic.xml > %TMPDIR%\topic_complet_tableaux.html

REM les 3 lignes suivantes peuvent être commentées lorsqu'on ne veut que l'affichage 1 (topic)
REM %COLORDIR%\manip.pl temp %1 factual > %TMPDIR%\%1.xip.xml
REM %COLORDIR%\create_html-fc.perl %TMPDIR%\%1.xip.xml > %TMPDIR%\factual_complet.html
REM %COLORDIR%\create_html-fc.perl %TMPDIR%\tableaux_factual.xml > %TMPDIR%\factual_complet_tableaux.html

%PERL% %COLORDIR%\create_html-ev.perl %TMPDIR%\%1.xip.xml > %TMPDIR%\evenements.html

copy cadre-ev.html %TMPDIR%
copy cadre-tcf.html %TMPDIR%
copy cadre-tc.html %TMPDIR%
copy cadre-tc-tableau.html %TMPDIR%
copy cadre-tf.html %TMPDIR%
copy demo.html %TMPDIR%
copy legende_ev.html %TMPDIR%
copy legende.html %TMPDIR%
copy legende_tc.html %TMPDIR%
copy legende_tf.html %TMPDIR%
copy liens.html %TMPDIR%
copy logoXRCE.jpg %TMPDIR%
copy X.jpg %TMPDIR%

