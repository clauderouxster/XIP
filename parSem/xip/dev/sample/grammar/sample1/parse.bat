echo off

set XIP_HOME=%~dp0\..\..\..

%XIP_HOME%\bin\windows\xip -grm sample.grm -tr -f %1 %2 %3 %4 %5

