echo on

del test.exe test.obj

set libDir="C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Lib"
set lib1=%libDir%\WS2_32.Lib
set lib2=%libDir%\User32.Lib

set root=..\..\..\..
set libSXip=%root%\xipFst\lib\windows\staticLib\xip.lib
set libDXip=%root%\xipFst\lib\windows\sharedLib\xip.lib

cl /c /nologo /EHsc /O2 /DNDEBUG /MD /I%root%\include test.cxx

echo ------- test (with static link) --------
link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:test.exe %lib1% %lib2% %libSXip% test.obj

echo ------- test (with dynamic link) --------
REM link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:test.exe %libDXip% test.obj

test
