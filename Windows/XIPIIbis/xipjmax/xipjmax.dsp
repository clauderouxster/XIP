# Microsoft Developer Studio Project File - Name="xipjmax" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xipjmax - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xipjmax.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xipjmax.mak" CFG="xipjmax - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xipjmax - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xipjmax - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xipjmax - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XIPJMAX_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\\pilat\fst\ntm\\" /I "\\truc\xelda\xelda-1.7\import\include" /I "d:\xip\jmax\src\exe\inc" /I "\\truc\roux\parseur\parsVect\dll" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XIPJMAX_EXPORTS" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "JMAX_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 \\truc\xelda\xelda-1.7\import\ix86-windowsNT4\lib\fsmFst.lib \\pilat\fst\ntm\windows\release\ntm.lib MSVCRT.lib d:\xip\jmax\src\exe\lib\jmax_win32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "xipjmax - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XIPJMAX_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "\\truc\roux\parseur\parsVect\dll" /I "d:\xip\jmax\src\exe\inc" /I "\\pilat\fst\ntm\\" /I "\\truc\xelda\xelda-1.7\import\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XIPJMAX_EXPORTS" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "JMAX_DLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 nafxcwd.lib \\truc\xelda\xelda-1.7\import\ix86-windowsNT4\lib\fsmFst_d.lib \\pilat\fst\ntm\windows\Debug\ntm_d.lib d:\xip\jmax\src\exe\lib\jmax_win32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xipjmax - Win32 Release"
# Name "xipjmax - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\affiche.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\analyse.cxx
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

SOURCE=C:\XIP\XIPIIbis\XIP\src\desamb\disamb.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\divers.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\desamb\hash.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\hmmexport.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\desamb\hmmread.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\initial.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\lire.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\lliste.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\desamb\memory.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\noeud.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\parseur.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\pls.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\printresult.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\result.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\serveur.cxx
# End Source File
# Begin Source File

SOURCE=..\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\traitement.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\vecteur.cxx
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\desamb\viterbi.c
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\desamb\words.c
# End Source File
# Begin Source File

SOURCE=..\xipdll.cpp
# End Source File
# Begin Source File

SOURCE=..\xipdll.def
# End Source File
# Begin Source File

SOURCE=..\xipdll.rc
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\xmlxip.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\xipjmax.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
