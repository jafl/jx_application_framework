# Microsoft Developer Studio Project File - Name="test_jcore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test_jcore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_jcore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_jcore.mak" CFG="test_jcore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_jcore - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_jcore - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_jcore - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\code" /I "..\..\misc\win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D SIZEOF_LONG=4 /D SIZEOF_INT=4 /D "PCRE_STATIC" /D "ACE_AS_STATIC_LIBS" /D "ACE_HAS_ACE_TOKEN" /D "ACE_HAS_ACE_SVCCONF" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shfolder.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "test_jcore - Win32 Release"
# Name "test_jcore - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc;th"
# Begin Source File

SOURCE=".\code\Everything-long.cpp"
# End Source File
# Begin Source File

SOURCE=".\code\JHashTable-strint.cpp"
# End Source File
# Begin Source File

SOURCE=".\code\JStringMap-int.cpp"
# End Source File
# Begin Source File

SOURCE=".\code\JStringPtrMap-char.cpp"
# End Source File
# Begin Source File

SOURCE=.\code\periodtestrand.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\sptest.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_bool.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_boolsub.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_FileExists.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JArray.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JBoolean.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JBroadcaster.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JDirInfo.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JFileArray.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_jFStreamUtil.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JLinkedList.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JMatrix.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JPipe.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JPrefsFile.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JPtrArray.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JQueue.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_jRandom.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JRunArray.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JStack.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JString.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JSubset.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JTextChooseSaveFile.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JTextProgressDisplay.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JTextUserNotification.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_JVector.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\test_stl.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testescape.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testhash.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testmap.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testparseargs.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testptrmap.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testrand.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testregex.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testsystem.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\testtable.cpp

!IF  "$(CFG)" == "test_jcore - Win32 Release"

!ELSEIF  "$(CFG)" == "test_jcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
