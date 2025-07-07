REM This is a tool to understand what is happening with Visual Studio 2005 (no PB with VS 2003)
REM "http://www.itwriting.com/blog/?postid=261"
REM http://www.codeproject.com/cpp/vcredists_x86.asp

REM set lib=..\lib\windows\sharedLibPy\xips.dll
set libD=build\lib\jXip\windows\sharedLib
set lib=%libD%\jXip.dll
set genM=%libD%\jxip.dll.manifest

REM to find dll dependencies
REM dumpbin /DEPENDENTS %lib%
depends %lib%

set extM=extracted.manifest
del *.manifest
REM to insert a manifest to a DLL
REM DLLs need to have its manifest embedded as a resource with the resource ID equal to 2 (according to MSDN).
REM "If you are receiving a R6034 error, then make sure your app contains a manifest. 
REM Then make sure your folder includes a second manifest called Microsoft.VC80.CRT.Manifest (see above).
REM Note that if you're running Windows 95/NT or below, then it's not possible to solve your error unless
REM you upgrade your operating system."

REM use to add or extract a manifest: ;#2 for a dll or ;#1 for an exe
REM mt /nologo /manifest %genM% /outputresource:%lib%;#2

REM to extract a manifest from a DLL
REM mt /nologo /out:%extM% /inputresource:%lib%;#2

REM type %extM%
REM echo "----"
REM type %genM%
