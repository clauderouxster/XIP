# Microsoft Developer Studio Project File - Name="xiplocal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=xiplocal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xiplocal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xiplocal.mak" CFG="xiplocal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xiplocal - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "xiplocal - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xiplocal - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\XIP\XIPIIbis\LanguageGuesser\include" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "XIPSTLUSE" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"C:\XIP\XIPIIbis\cfsmlib7.00" /libpath:"C:\XIP\XIPIIbis\LanguageGuesser\langlib\Release"

!ELSEIF  "$(CFG)" == "xiplocal - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\XIP\xipdll\ntm\include" /I "C:\XIP\xipdll\ntm\include\ntm" /I "C:\XIP\XIPIIbis\XIP\include" /I "C:\XIP\XIPIIbis\LanguageGuesser\include" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "NTMCONNECTION" /D "XIPDLL_EXPORT" /D "XMLINPUT" /D "XIPSTLAPI" /D "_AFXDLL" /D "WIN32" /D "XIPSTLUSE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fsmFst_d.lib ntm_d.lib libxml2_a.lib ws2_32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"C:\XIP\libxml2-2.5.1\win32\dsp\libxml2" /libpath:"C:\XIP\XIPIIbis\cfsmlib7.00" /libpath:"C:\XIP\XIPIIbis\LanguageGuesser\langlib\Release"

!ENDIF 

# Begin Target

# Name "xiplocal - Win32 Release"
# Name "xiplocal - Win32 Debug"
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

SOURCE=R:\XIPIIbis\src\automate.cxx
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

SOURCE=..\..\exemple\example.cxx
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

SOURCE=C:\XIP\XIPIIbis\XIP\src\readxml.cxx
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

SOURCE=C:\XIP\XIPIIbis\XIP\src\stlresul.cxx
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

SOURCE=C:\XIP\XIPIIbis\XIP\src\xiplibxml.cxx
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
