@echo on
del clef.exe

REM set libDir="C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Lib"
set libDir="C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Lib"
set lib1=%libDir%\WS2_32.Lib
set lib2=%libDir%\User32.Lib

cl /c /nologo /EHsc /O2 /DNDEBUG /MD /I..\include ..\src\clef.cxx
link /NOLOGO /SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OUT:clef.exe %lib1% %lib2% clef.obj
mt /nologo /manifest clef.exe.manifest /outputresource:clef.exe;#1

del clef.exe.manifest



