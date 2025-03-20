# Microsoft Developer Studio Project File - Name="ntmdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ntmdll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ntmdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ntmdll.mak" CFG="ntmdll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ntmdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ntmdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ntmdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ntmdll___Win32_Release"
# PROP Intermediate_Dir "ntmdll___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NTMDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\\Smbhost\Mltt\c-fsm\i586-linux2.0\src\fst" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32" /D "_FSMFSTDLL" /D "_FSMTOKENIZEDLL" /D "_FSMLOOKUPDLL" /D "_FSMNTMDLL" /D "_FSMNTMBUILDDLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 fsmFst.lib fsmTokenize.lib fsmLookup.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /libpath:"Y:\ix86-windowsNT4-vc6\lib" /libpath:"X:\xelda-1.8\import\ix86-windowsNT4\lib" /libpath:"X:\xelda-1.7\import\ix86-windowsNT4\lib"

!ELSEIF  "$(CFG)" == "ntmdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ntmdll___Win32_Debug"
# PROP BASE Intermediate_Dir "ntmdll___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ntmdll___Win32_Debug"
# PROP Intermediate_Dir "ntmdll___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NTMDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "\\Smbhost\Mltt\c-fsm\i586-linux2.0\src\fst" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_WIN32" /D "_FSMFSTDLL" /D "_FSMTOKENIZEDLL" /D "_FSMLOOKUPDLL" /D "_FSMNTMDLL" /D "_FSMNTMBUILDDLL" /FR /YX /Fd"ntmdll___Win32_Debug/ntmdll_d.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fsmFst_d.lib fsmTokenize_d.lib fsmLookup_d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"ntmdll___Win32_Debug/ntmdll_d.dll" /pdbtype:sept /libpath:"Y:\ix86-windowsNT4-vc6\lib" /libpath:"X:\xelda-1.8\import\ix86-windowsNT4\lib" /libpath:"X:\xelda-1.7\import\ix86-windowsNT4\lib"

!ENDIF 

# Begin Target

# Name "ntmdll - Win32 Release"
# Name "ntmdll - Win32 Debug"
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

SOURCE=.\ntmstdinputobject.c
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

SOURCE=.\ntmstdinputobject.h
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
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
