call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64\vcvars64.bat"

set JAVA_HOME=C:\Program Files\Java\jdk1.7.0_40
set Path=C:\Program Files\Java\jdk1.7.0_40\bin;C:\Program Files\Debugging Tools for Windows (x64);w:\tools\ant\1.8.2\bin;%Path%
set ANT_HOME=w:\tools\ant\1.8.2
call ant sharedLib.debug
cd java
call compileidad.bat
cd W:\users\Roux\Imagiweb
call compileida.bat




