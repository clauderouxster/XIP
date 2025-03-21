@echo off
REM usage:
REM        echo "this is a test" | xipparse
REM       xipparse -text fileNameToParse

REM set XIP_HOME to the installation directory of XIP
REM set XIP_HOME to an absolute path if you want to launch xipparse from anywhere
set XIP_HOME=%~dp0\..\..

set GRMFILE=%XIP_HOME%\grammar\english\entity\gram_gen_entit.grm

call %XIP_HOME%\bin\windows\xip.exe -grm %GRMFILE% -tr -f -t %1 %2 %3 %4 %5
