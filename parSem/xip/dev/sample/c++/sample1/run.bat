echo on
set root=..\..\..
REM UPDATE THE NEXT LINE ACCORDING TO YOUR VISUAL STUDIO INSTALLATION AND YOUR PLATFORM
REM chevreuse (x86)
set vsRoot11=C:\Program Files\Microsoft Visual Studio 8
REM Xip is not compiled on X86 with VS 9 so you can't use the next line
REM set vsRoot12=C:\Program Files\Microsoft Visual Studio 9  
REM Xip is not compiled on X86 with VS 10.0 so you can't use the next line
REM set vsRoot13=C:\Program Files\Microsoft Visual Studio 10.0
set xipLibDir1=%root%\lib\windows
set arch1=

REM dolent (amd64)
REM on amd64 machine XIP can be used only with VS 10.0
set vsRoot2=C:\Program Files (x86)\Microsoft Visual Studio 10.0
set xipLibDir2=%root%\lib\windows64
set arch2=amd64

REM avoriaz (x86)
REM on x86 machine XIP can be used only with VS 8
set vsRoot3=C:\Program Files\Microsoft Visual Studio 8
set xipLibDir3=%root%\lib\windows
set arch3=

REM select one platform from the previous list
set vsRoot=%vsRoot3%
set xipLibDir=%xipLibDir3%
set arch=%arch3%

set libDir=%vsRoot%\VC\PlatformSDK\Lib

set libSXip=%xipLibDir%\staticLib\xip.lib 
set libDXip=%xipLibDir%\sharedLib\xip.lib 

set GRM=%root%\grammar\english\norm\gram_norm_entit.grm

set inputFile=input.txt

REM Visual Studio env setting:
call "%vsRoot%\VC\vcvarsall.bat" %arch%
echo on

del sampleS sampleD *.exe *.obj *.lib *.exp *.dll *.manifest output /Q

cl /c /nologo /EHsc /O2 /DNDEBUG /MD /I%root%\include sample.cxx

echo ------- sample (with static link) --------
link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:sampleS.exe %libSXip% sample.obj
sampleS %GRM% %inputFile%

echo ------- sample (with dynamic link) --------
copy %xipLibDir%\sharedLib\xip.dll .
link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:sampleD.exe %libDXip% sample.obj
sampleD %GRM% %inputFile%

