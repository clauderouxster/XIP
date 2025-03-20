# Microsoft Developer Studio Project File - Name="xipxstl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xipxstl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xipxstl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xipxstl.mak" CFG="xipxstl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xipxstl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xipxstl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xipxstl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XIPXSTL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "C:\XIP\XIPIIbis\XIP\include" /I "c:\xip\XIPIIbis\cfsm-1.1.1\include\fst" /I "c:\XIP\XIPIIbis\XIP\include" /I "c:\xip\XIPIIbis\cfsm-1.1.1\include" /I "c:\xip\XIPIIbis\cfsm-1.1.1\include\ntm" /I "C:\XIP\libxml2-2.6.14\include" /I "C:\XIP\XIPIIbis\LanguageGuesser\include" /I "C:\Python24\include" /D "NDEBUG" /D "_WINDLL" /D "_WINDOWS" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "XMLINPUT" /D "XIPSTLAPI" /D "XIPLIBXML" /D "_AFXDLL" /D "WIN32" /D "XIPSTLUSE" /D "_FSMFSTDLL" /D "XIPPYTHON" /D "WIN32_DUPLICATE_FST" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 fsmFst.lib ntm.lib python24.lib libxml2_a.lib ws2_32.lib langlib.lib /nologo /dll /machine:I386 /libpath:"C:\XIP\libxml2-2.6.14\win32\dsp\libxml2r" /libpath:"C:\XIP\XIPIIbis\cfsm-1.1.1" /libpath:"C:\XIP\XIPIIbis\LanguageGuesser\langlib\Release" /libpath:"C:\Python24\libs"
# Begin Custom Build
InputPath=.\Release\xipxstl.dll
SOURCE="$(InputPath)"

"release\xipxstl_a.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	lib /nologo /out:release\xipxstl_a.lib release\*.obj C:\XIP\libxml2-2.5.1\win32\dsp\libxml2r\libxml2_a.lib c:\XIP\xipdll\ntm\8.0.0\lib\fsmfst.lib c:\XIP\xipdll\ntm\8.0.0\lib\ntm.lib ws2_32.lib

# End Custom Build

!ELSEIF  "$(CFG)" == "xipxstl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XIPXSTL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\xip\XIPIIbis\cfsm-1.1.1\include\fst" /I "c:\XIP\XIPIIbis\XIP\include" /I "c:\xip\XIPIIbis\cfsm-1.1.1\include" /I "c:\xip\XIPIIbis\cfsm-1.1.1\include\ntm" /I "C:\XIP\libxml2-2.6.14\include" /I "C:\XIP\XIPIIbis\LanguageGuesser\include" /I "C:\Python24\include" /D "_DEBUG" /D "_WINDLL" /D "_WINDOWS" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "XMLINPUT" /D "XIPSTLAPI" /D "XIPLIBXML" /D "_AFXDLL" /D "WIN32" /D "XIPSTLUSE" /D "_FSMFSTDLL" /D "XIPPYTHON" /D "WIN32_DUPLICATE_FST" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fsmFst_d.lib ntm_d.lib python24_d.lib libxml2_a.lib ws2_32.lib langlib.lib /nologo /dll /debug /machine:I386 /out:"Debug/xipxstld.dll" /pdbtype:sept /libpath:"C:\XIP\libxml2-2.6.14\win32\dsp\libxml2" /libpath:"C:\XIP\XIPIIbis\cfsm-1.1.1" /libpath:"C:\XIP\XIPIIbis\LanguageGuesser\langlib\Release" /libpath:"C:\Python24\libs"

!ENDIF 

# Begin Target

# Name "xipxstl - Win32 Release"
# Name "xipxstl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\affiche.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\analyse.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\applique.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\automate.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\avl.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\avlcat.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\avlfonc.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\avlreg.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\avltrait.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\avlvoc.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\chaineen.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\chainefr.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\disamb.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\divers.cxx
# End Source File
# Begin Source File

SOURCE=..\XIP\src\ems.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\indexation.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\initial.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\lecttest.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\lire.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\lliste.cxx
# End Source File
# Begin Source File

SOURCE=..\XIP\src\lookup.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\mathemat.cxx
# End Source File
# Begin Source File

SOURCE=..\XIP\src\networks.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\noeud.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\ntm.cxx
# End Source File
# Begin Source File

SOURCE=..\XIP\src\ontologies.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\parseur.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\pls.cxx
# End Source File
# Begin Source File

SOURCE=..\XIP\src\readscript.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\result.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\serveur.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\sinput.cxx
# End Source File
# Begin Source File

SOURCE=..\xipstl\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\stlresul.cxx
# End Source File
# Begin Source File

SOURCE="..\XIP\src\tok-aux.c"
# End Source File
# Begin Source File

SOURCE=..\XIP\src\tokenplus.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\traitement.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\varlibxml.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\vecteur.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\xipaxip.cxx
# End Source File
# Begin Source File

SOURCE=..\xipdllpure\xipdllpure.rc
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\xiplibxml.cxx
# End Source File
# Begin Source File

SOURCE=..\xipstl\xipstl.cpp
# End Source File
# Begin Source File

SOURCE=.\xipxstl.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\xmlxip.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
