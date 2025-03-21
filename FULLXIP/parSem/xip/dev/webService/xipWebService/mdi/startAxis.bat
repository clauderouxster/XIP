@echo on
set JAVA_HOME=c:\Program Files\Java\jdk1.6.0_06
set PATH=%JAVA_HOME%\bin;%PATH%
set AXIS2_HOME=c:\users\grondeau\axis2-1.4
set PATH=%AXIS2_HOME%\bin;%PATH%

call axis2server.bat
