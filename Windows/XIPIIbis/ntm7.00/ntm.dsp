# Microsoft Developer Studio Project File - Name="ntm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ntm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ntm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ntm.mak" CFG="ntm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ntm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ntm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ntm - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\\Smbhost\Mltt\c-fsm\i586-linux2.0\src\fst" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_WIN32" /D "_WINDOWS" /D "_FSMFSTDLL" /D "_FSMTOKENIZEDLL" /D "_FSMLOOKUPDLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ntm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\\Smbhost\Mltt\c-fsm\i586-linux2.0\src\fst" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_WIN32" /D "_WINDOWS" /D "_FSMFSTDLL" /D "_FSMTOKENIZEDLL" /D "_FSMLOOKUPDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\ntm_d.lib"

!ENDIF 

# Begin Target

# Name "ntm - Win32 Release"
# Name "ntm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ntm.c
# End Source File
# Begin Source File

SOURCE=.\ntmlevels.c
# End Source File
# Begin Source File

SOURCE=.\ntmload.c
# End Source File
# Begin Source File

SOURCE=.\ntmmemory.c
# End Source File
# Begin Source File

SOURCE=.\ntmmorpho.c
# End Source File
# Begin Source File

SOURCE=.\ntmtoken.c
# End Source File
# Begin Source File

SOURCE=.\ntmtypo.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ntm.h
# End Source File
# Begin Source File

SOURCE=.\ntm_windefs.h
# End Source File
# Begin Source File

SOURCE=.\ntmapp.h
# End Source File
# Begin Source File

SOURCE=.\ntmextern.h
# End Source File
# Begin Source File

SOURCE=.\ntmlevels.h
# End Source File
# Begin Source File

SOURCE=.\ntmmemory.h
# End Source File
# Begin Source File

SOURCE=.\ntmtoken.h
# End Source File
# Begin Source File

SOURCE=.\ntmtypes.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# End Target
# End Project
