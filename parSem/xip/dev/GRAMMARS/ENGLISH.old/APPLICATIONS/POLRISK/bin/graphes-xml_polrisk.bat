@echo off
set ROOTDIR=C:\users\aude\xip-9.62-01

set GRM=%ROOTDIR%\GRAMMARS\ENGLISH\APPLICATIONS\POLRISK\data\english-graphes_pr_xml.grm

set XIP=%ROOTDIR%\bin\windows\xip

%XIP% -grm %GRM% -tr -f -sentence -sentencenum -xmltext %1 %2 %3 %4 %5 %6 %7
