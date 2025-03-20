# Microsoft Developer Studio Project File - Name="xipdllpure" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xipdllpure - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xipdllpure.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xipdllpure.mak" CFG="xipdllpure - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xipdllpure - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xipdllpure - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xipdllpure - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "c:\XIP\XIPIIbis\XIP\include" /I "c:\xip\cfsm7.00\include" /I "c:\xip\cfsm7.00\include\ntm" /I "c:\xip\cfsm7.00\include\fst" /D "NDEBUG" /D "_MBCS" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "XMLINPUT" /D "WIN32" /D "XIPSTLUSE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 fsmFst.lib ntm.lib MSVCRT.lib langlib.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/xipdll.dll" /libpath:"C:\XIP\XIPIIbis\cfsmlib1.0.9" /libpath:"C:\XIP\XIPIIbis\LanguageGuesser\langlib\Release"

!ELSEIF  "$(CFG)" == "xipdllpure - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\XIP\libxml2-2.5.1\include" /I "c:\XIP\XIPIIbis\XIP\include" /I "c:\xip\cfsm7.00\include" /I "c:\xip\cfsm7.00\include\ntm" /I "c:\xip\cfsm7.00\include\fst" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "XMLINPUT" /D "WIN32" /D "XIPSTLUSE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRTD.lib fsmFst_d.lib ntm_d.lib langlib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug\xipdll.dll" /pdbtype:sept /libpath:"C:\XIP\XIPIIbis\cfsmlib1.0.9" /libpath:"C:\XIP\XIPIIbis\LanguageGuesser\langlib\Release"

!ENDIF 

# Begin Target

# Name "xipdllpure - Win32 Release"
# Name "xipdllpure - Win32 Debug"
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

SOURCE=C:\XIP\XIPIIbis\XIP\src\mathemat.cxx
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

SOURCE=C:\XIP\XIPIIbis\XIP\src\printresult.cxx
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

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
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

SOURCE=.\xipdllpure.cpp
# End Source File
# Begin Source File

SOURCE=.\xipdllpure.def
# End Source File
# Begin Source File

SOURCE=.\xipdllpure.rc
# End Source File
# Begin Source File

SOURCE=C:\XIP\XIPIIbis\XIP\src\xmlxip.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\xipdllpure.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\xipdllpure.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
