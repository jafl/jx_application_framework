# Microsoft Developer Studio Project File - Name="X" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=X - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "X.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "X.mak" CFG="X - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "X - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "X - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "X - Win32 Release"

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
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"XStdInc.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "X - Win32 Debug"

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
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /Gy /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"XStdInc.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "X - Win32 Release"
# Name "X - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\display.cpp
# End Source File
# Begin Source File

SOURCE=.\keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\pixmap.cpp
# End Source File
# Begin Source File

SOURCE=.\private.cpp
# End Source File
# Begin Source File

SOURCE=.\region.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\window.cpp
# End Source File
# Begin Source File

SOURCE=.\XStdInc.cpp

!IF  "$(CFG)" == "X - Win32 Release"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "X - Win32 Debug"

# ADD CPP /Yc"XStdInc.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\X11\cursorfont.h
# End Source File
# Begin Source File

SOURCE=.\X11\keysym.h
# End Source File
# Begin Source File

SOURCE=.\X11\keysymdef.h
# End Source File
# Begin Source File

SOURCE=.\private.h
# End Source File
# Begin Source File

SOURCE=.\X11\extensions\shape.h
# End Source File
# Begin Source File

SOURCE=.\X11\X.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xatom.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xfuncproto.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xlib.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xmd.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xosdefs.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xproto.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xprotostr.h
# End Source File
# Begin Source File

SOURCE=.\XStdInc.h
# End Source File
# Begin Source File

SOURCE=.\X11\Xutil.h
# End Source File
# End Group
# End Target
# End Project
