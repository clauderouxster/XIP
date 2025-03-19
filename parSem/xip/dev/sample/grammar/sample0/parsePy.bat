echo off

set XIP_HOME=%~dp0\..\..\..

%XIP_HOME%\bin\windows\xips -grm gramPy.grm -tr -f %1 %2 %3 %4 %5

