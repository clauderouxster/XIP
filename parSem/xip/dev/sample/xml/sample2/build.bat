echo on

call clean

REM set libDir="C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Lib"
set libDir="C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Lib"
set lib1=%libDir%\WS2_32.Lib
set lib2=%libDir%\User32.Lib

set root=..\..\..
set libSXip=%root%\lib\windows\staticLib\xip.lib 
set libDXip=%root%\lib\windows\sharedLib\xip.lib 

cl /c /nologo /EHsc /O2 /DNDEBUG /MD /I%root%\include sample.cxx

echo ------- sample (with static link) --------
link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:sample.exe %lib1% %lib2% %libSXip% sample.obj

echo ------- sample (with dynamic link) --------
REM link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:sample.exe %libDXip% sample.obj
