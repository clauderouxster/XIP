@echo on
REM set XIP_HOME to the installation directory of XIP
REM set XIP_HOME to an absolute path if you want to launch xipparse from anywhere
set XIP_HOME=%~dp0\..\..

REM Update ROOT_XIP to select the grammar you want to use
set ROOT_XIP=%XIP_HOME%\GRAMMARS
set GRMFILE=%XIP_HOME%\GRAMMARS\ENGLISH\GRMFILES\GRAM_GEN\gram_gen_entit.grm
REM set GRMFILE=%XIP_HOME%\GRAMMARS\ENGLISH\APPLICATIONS\XipTriplets\GRMFILES_trip\gram_norm_trip.grm
REM set GRMFILE=%XIP_HOME%\GRAMMARS\GERMAN\GRMFILES\german.grm
copy %XIP_HOME%\..\..\python\2.4.1\windows\python24.dll .

call Release\xipwin.exe %GRMFILE%
