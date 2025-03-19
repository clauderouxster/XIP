@echo off
set ROOTDIR=C:\parSem\demo-EventSpotter\xip-9.62-01

set GRM=%ROOTDIR%\GRAMMARS\ENGLISH\APPLICATIONS\POLRISK\data\english_pr.grm

set XIP=%ROOTDIR%\bin\windows\xip

%XIP% -grm %GRM% -tr -f -sentence -sentencenum %1 %2 %3 %4 %5 %6 %7
