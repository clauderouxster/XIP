# Microsoft Developer Studio Project File - Name="xipwin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xipwin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xipwin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xipwin.mak" CFG="xipwin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xipwin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xipwin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xipwin - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NTMCONNECTION" /D "XIPSTLUSE" /U "XIPSTLAPI" /U "XIPLIBXML" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\lib\windows\staticLibPy\xips.lib ..\..\..\..\python\2.4.1\windows\python24.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/lingdexip.exe"

!ELSEIF  "$(CFG)" == "xipwin - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "NTMCONNECTION" /D "XIPSTLUSE" /U "XIPSTLAPI" /U "XIPLIBXML" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\lib\windows\staticLib\xip.lib   ws2_32.lib kernel32.lib user32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xipwin - Win32 Release"
# Name "xipwin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Featuretree.cpp
# End Source File
# Begin Source File

SOURCE=.\filenameinput.cpp
# End Source File
# Begin Source File

SOURCE=.\Filetree.cpp
# End Source File
# Begin Source File

SOURCE=.\generate.cpp
# End Source File
# Begin Source File

SOURCE=.\helpscripting.cpp
# End Source File
# Begin Source File

SOURCE=.\inforule.cpp
# End Source File
# Begin Source File

SOURCE=.\limit.cpp
# End Source File
# Begin Source File

SOURCE=.\ListeVariable.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ncoord.cpp
# End Source File
# Begin Source File

SOURCE=.\optionsXip.cpp
# End Source File
# Begin Source File

SOURCE=.\ParametreFile.cpp
# End Source File
# Begin Source File

SOURCE=.\printvar.cpp
# End Source File
# Begin Source File

SOURCE=.\Resdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\resultat.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleBuilding.cpp
# End Source File
# Begin Source File

SOURCE=.\ruledoc.cpp
# End Source File
# Begin Source File

SOURCE=.\saisiedoc.cpp
# End Source File
# Begin Source File

SOURCE=.\saisieview.cpp
# End Source File
# Begin Source File

SOURCE=.\Searchstring.cpp
# End Source File
# Begin Source File

SOURCE=.\seerule.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\xipwin.cpp
# End Source File
# Begin Source File

SOURCE=.\xipwin.rc
# End Source File
# Begin Source File

SOURCE=.\xipwinDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\xipwinView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\Featuretree.h
# End Source File
# Begin Source File

SOURCE=.\filenameinput.h
# End Source File
# Begin Source File

SOURCE=.\Filetree.h
# End Source File
# Begin Source File

SOURCE=.\generate.h
# End Source File
# Begin Source File

SOURCE=.\helpscripting.h
# End Source File
# Begin Source File

SOURCE=.\inforule.h
# End Source File
# Begin Source File

SOURCE=.\limit.h
# End Source File
# Begin Source File

SOURCE=.\ListeVariable.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ncoord.h
# End Source File
# Begin Source File

SOURCE=.\optionsXip.h
# End Source File
# Begin Source File

SOURCE=.\ParametreFile.h
# End Source File
# Begin Source File

SOURCE=.\printvar.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\result.h
# End Source File
# Begin Source File

SOURCE=.\Resultat.h
# End Source File
# Begin Source File

SOURCE=.\RuleBuilding.h
# End Source File
# Begin Source File

SOURCE=.\ruledoc.h
# End Source File
# Begin Source File

SOURCE=.\saisiedoc.h
# End Source File
# Begin Source File

SOURCE=.\saisieview.h
# End Source File
# Begin Source File

SOURCE=.\Searchstring.h
# End Source File
# Begin Source File

SOURCE=.\seerule.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\vecta.h
# End Source File
# Begin Source File

SOURCE=.\xipwin.h
# End Source File
# Begin Source File

SOURCE=.\xipwinDoc.h
# End Source File
# Begin Source File

SOURCE=.\xipwinView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\rules_ed.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xipwin.ico
# End Source File
# Begin Source File

SOURCE=.\res\xipwin.rc2
# End Source File
# Begin Source File

SOURCE=.\res\xipwinDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
