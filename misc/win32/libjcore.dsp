# Microsoft Developer Studio Project File - Name="libjcore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libjcore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libjcore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libjcore.mak" CFG="libjcore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libjcore - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libjcore - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libjcore - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "code" /I "..\misc\win32" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"JCoreStdInc.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /Gy /I "code" /I "..\misc\win32" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D SIZEOF_LONG=4 /D SIZEOF_INT=4 /D "PCRE_STATIC" /D "ACE_HAS_ACE_TOKEN" /D "ACE_HAS_ACE_SVCCONF" /D "ACE_AS_STATIC_LIBS" /FR /Yu"JCoreStdInc.h" /FD /GZ /c
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

# Name "libjcore - Win32 Release"
# Name "libjcore - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc;th"
# Begin Source File

SOURCE=.\code\jAssert.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JAssertBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JBooleanIO.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JBroadcaster.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JBroadcastSnooper.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JChooseSaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jColor.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JColorList.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JColormap.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jCommandLine.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JComplex.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JCoreStdInc.cpp
# ADD CPP /Yc"JCoreStdInc.h"
# End Source File
# Begin Source File

SOURCE=.\code\JCreateProgressDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JCreateTextPG.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JDirEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JDirInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JDirInfo_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JDirInfo_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jDirUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jDirUtil_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jDirUtil_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JEPSPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JError.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JErrorState.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JExtractHTMLTitle.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFileArray.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFileArrayIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFileID.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jFileUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jFileUtil_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jFileUtil_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFloatBufferTableData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFloatTableData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFontManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JFontStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jFStreamUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jFStreamUtil_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jFStreamUtil_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JGetCurrentColormap.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JGetCurrentFontManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jGlobals_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jGlobals_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jHashFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JHTMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JHTMLScannerL.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JImage.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JImageMask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JIndexRange.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JInterpolate.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JIntRange.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JKLRand.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JLatentPG.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jMath.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JMDIServer.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jMountUtil_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jMountUtil_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jMouseUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JNamedTreeList.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JNamedTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSetT.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSetUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JOutPipeStream.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPagePrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPainter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPrefObject.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPrefsFile.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPrefsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JProbDistr.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JProcessError.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jProcessUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jProcessUtil_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jProcessUtil_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JProgressDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPSPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPSPrinterBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JPTPrinter.cpp
# End Source File
# Begin Source File

SOURCE=".\code\JPtrArray-JString.cpp"
# End Source File
# Begin Source File

SOURCE=.\code\jRand.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JRegex.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JRTTIBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jSignal.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JSimpleProcess.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JSliderBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JStdError.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JStopWatch.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jStreamUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jStreamUtil_UNIX.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JString.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JStringManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JStringTableData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JStyleTableData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JSubset.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JSubstitute.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jSysUtil_UNIX.cpp

!IF  "$(CFG)" == "libjcore - Win32 Release"

!ELSEIF  "$(CFG)" == "libjcore - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\jSysUtil_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTableData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTableSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTableSelectionIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEHTMLScanner.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTESetCurrentFont.th
# End Source File
# Begin Source File

SOURCE=.\code\JTESetFont.th
# End Source File
# Begin Source File

SOURCE=.\code\JTEStyler.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoPaste.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoTabShift.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoTextBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoTyping.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTextChooseSaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTextEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTextProgressDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTextUserNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JThisProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jTime.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jTime_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTree.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTreeList.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JUserNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JVector.cpp
# End Source File
# Begin Source File

SOURCE=".\code\Templates-double.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-float.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-int.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-bool.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-JCharacter.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-JString.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-JString2.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-JTextEditor.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-long.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-short.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-uint.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\code\Templates-ulong.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\code\JAliasArray.h
# End Source File
# Begin Source File

SOURCE=.\code\JArray.h
# End Source File
# Begin Source File

SOURCE=.\code\jASCIIConstants.h
# End Source File
# Begin Source File

SOURCE=.\code\jAssert.h
# End Source File
# Begin Source File

SOURCE=.\code\JAssertBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JAsynchDataReceiver.h
# End Source File
# Begin Source File

SOURCE=.\code\JAuxTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JBroadcaster.h
# End Source File
# Begin Source File

SOURCE=.\code\JBroadcastSnooper.h
# End Source File
# Begin Source File

SOURCE=.\code\JChooseSaveFile.h
# End Source File
# Begin Source File

SOURCE=.\code\JCollection.h
# End Source File
# Begin Source File

SOURCE=.\code\jColor.h
# End Source File
# Begin Source File

SOURCE=.\code\JColorList.h
# End Source File
# Begin Source File

SOURCE=.\code\JColormap.h
# End Source File
# Begin Source File

SOURCE=.\code\jCommandLine.h
# End Source File
# Begin Source File

SOURCE=.\code\JComplex.h
# End Source File
# Begin Source File

SOURCE=.\code\JConstBitmap.h
# End Source File
# Begin Source File

SOURCE=.\code\JConstHashCursor.h
# End Source File
# Begin Source File

SOURCE=.\code\JContainer.h
# End Source File
# Begin Source File

SOURCE=.\code\JCoreLibVersion.h
# End Source File
# Begin Source File

SOURCE=.\code\JCoreStdInc.h
# End Source File
# Begin Source File

SOURCE=.\code\JCreateProgressDisplay.h
# End Source File
# Begin Source File

SOURCE=.\code\JCreateTextPG.h
# End Source File
# Begin Source File

SOURCE=.\code\JDirEntry.h
# End Source File
# Begin Source File

SOURCE=.\code\JDirInfo.h
# End Source File
# Begin Source File

SOURCE=.\code\jDirUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JDynamicHistogram.h
# End Source File
# Begin Source File

SOURCE=.\code\JEPSPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\jErrno.h
# End Source File
# Begin Source File

SOURCE=.\code\JError.h
# End Source File
# Begin Source File

SOURCE=.\code\JErrorState.h
# End Source File
# Begin Source File

SOURCE=.\code\JExtractHTMLTitle.h
# End Source File
# Begin Source File

SOURCE=.\code\JFAID.h
# End Source File
# Begin Source File

SOURCE=.\code\JFAIndex.h
# End Source File
# Begin Source File

SOURCE=.\code\JFileArray.h
# End Source File
# Begin Source File

SOURCE=.\code\JFileArrayIndex.h
# End Source File
# Begin Source File

SOURCE=.\code\JFileID.h
# End Source File
# Begin Source File

SOURCE=.\code\jFileUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JFloatBufferTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JFloatTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JFontManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JFontStyle.h
# End Source File
# Begin Source File

SOURCE=.\code\jFStreamUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JGetCurrentColormap.h
# End Source File
# Begin Source File

SOURCE=.\code\JGetCurrentFontManager.h
# End Source File
# Begin Source File

SOURCE=.\code\jGlobals.h
# End Source File
# Begin Source File

SOURCE=.\code\JHashCursor.h
# End Source File
# Begin Source File

SOURCE=.\code\jHashFunctions.h
# End Source File
# Begin Source File

SOURCE=.\code\JHashRecord.h
# End Source File
# Begin Source File

SOURCE=.\code\JHashTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JHistogram.h
# End Source File
# Begin Source File

SOURCE=.\code\JHTMLScanner.h
# End Source File
# Begin Source File

SOURCE=.\code\JImage.h
# End Source File
# Begin Source File

SOURCE=.\code\JImageMask.h
# End Source File
# Begin Source File

SOURCE=.\code\JIndexRange.h
# End Source File
# Begin Source File

SOURCE=.\code\JInPipeStream.h
# End Source File
# Begin Source File

SOURCE=.\code\JInterpolate.h
# End Source File
# Begin Source File

SOURCE=.\code\JIntRange.h
# End Source File
# Begin Source File

SOURCE=.\code\JKLRand.h
# End Source File
# Begin Source File

SOURCE=.\code\JLatentPG.h
# End Source File
# Begin Source File

SOURCE=.\code\JLinkedList.h
# End Source File
# Begin Source File

SOURCE=.\code\JLinkedListIterator.h
# End Source File
# Begin Source File

SOURCE=.\code\jMath.h
# End Source File
# Begin Source File

SOURCE=.\code\JMatrix.h
# End Source File
# Begin Source File

SOURCE=.\code\JMDIServer.h
# End Source File
# Begin Source File

SOURCE=.\code\jMemory.h
# End Source File
# Begin Source File

SOURCE=.\code\JMemoryManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JMessageProtocol.h
# End Source File
# Begin Source File

SOURCE=.\code\JMinMax.h
# End Source File
# Begin Source File

SOURCE=..\misc\win32\jMissingProto.h
# End Source File
# Begin Source File

SOURCE=.\code\JMMArrayTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JMMErrorPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JMMHashTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JMMMonitor.h
# End Source File
# Begin Source File

SOURCE=.\code\JMMRecord.h
# End Source File
# Begin Source File

SOURCE=.\code\JMMTable.h
# End Source File
# Begin Source File

SOURCE=.\code\jMountUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\jMouseUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JNamedTreeList.h
# End Source File
# Begin Source File

SOURCE=.\code\JNamedTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\code\JNetworkProtocolBase.h
# End Source File
# Begin Source File

SOURCE=.\code\jNew.h
# End Source File
# Begin Source File

SOURCE=.\code\JObjTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSet.h
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSetIterator.h
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSetUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JOutPipeStream.h
# End Source File
# Begin Source File

SOURCE=.\code\JOutPipeStreambuf.h
# End Source File
# Begin Source File

SOURCE=.\code\JPackedTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JPagePrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JPainter.h
# End Source File
# Begin Source File

SOURCE=.\code\JPartition.h
# End Source File
# Begin Source File

SOURCE=.\code\JPoint.h
# End Source File
# Begin Source File

SOURCE=.\code\JPolygon.h
# End Source File
# Begin Source File

SOURCE=.\code\JPrefObject.h
# End Source File
# Begin Source File

SOURCE=.\code\JPrefsFile.h
# End Source File
# Begin Source File

SOURCE=.\code\JPrefsManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JProbDistr.h
# End Source File
# Begin Source File

SOURCE=.\code\JProcess.h
# End Source File
# Begin Source File

SOURCE=.\code\JProcessError.h
# End Source File
# Begin Source File

SOURCE=.\code\jProcessUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JProgressDisplay.h
# End Source File
# Begin Source File

SOURCE=.\code\JPSPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JPSPrinterBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JPTPrinter.h
# End Source File
# Begin Source File

SOURCE=".\code\JPtrArray-JString.h"
# End Source File
# Begin Source File

SOURCE=.\code\JPtrArray.h
# End Source File
# Begin Source File

SOURCE=.\code\JPtrQueue.h
# End Source File
# Begin Source File

SOURCE=.\code\JPtrStack.h
# End Source File
# Begin Source File

SOURCE=.\code\JQueue.h
# End Source File
# Begin Source File

SOURCE=.\code\jRand.h
# End Source File
# Begin Source File

SOURCE=.\code\JRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JRegex.h
# End Source File
# Begin Source File

SOURCE=.\code\JRTTIBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JRunArray.h
# End Source File
# Begin Source File

SOURCE=.\code\JRunArrayIterator.h
# End Source File
# Begin Source File

SOURCE=.\code\jSignal.h
# End Source File
# Begin Source File

SOURCE=.\code\JSimpleProcess.h
# End Source File
# Begin Source File

SOURCE=.\code\JSliderBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JStack.h
# End Source File
# Begin Source File

SOURCE=.\code\JStdError.h
# End Source File
# Begin Source File

SOURCE=.\code\JStopWatch.h
# End Source File
# Begin Source File

SOURCE=.\code\jStreamUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JString.h
# End Source File
# Begin Source File

SOURCE=.\code\jStringData.h
# End Source File
# Begin Source File

SOURCE=.\code\JStringManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JStringMap.h
# End Source File
# Begin Source File

SOURCE=.\code\JStringMapCursor.h
# End Source File
# Begin Source File

SOURCE=.\code\JStringPtrMap.h
# End Source File
# Begin Source File

SOURCE=.\code\JStringPtrMapCursor.h
# End Source File
# Begin Source File

SOURCE=.\code\JStringTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JStrValue.h
# End Source File
# Begin Source File

SOURCE=.\code\JStyleTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JSubset.h
# End Source File
# Begin Source File

SOURCE=.\code\JSubstitute.h
# End Source File
# Begin Source File

SOURCE=.\code\jSysUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JTableSelection.h
# End Source File
# Begin Source File

SOURCE=.\code\JTableSelectionIterator.h
# End Source File
# Begin Source File

SOURCE=.\code\JTaskIterator.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEHTMLScanner.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEStyler.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoDrop.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoPaste.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoStyle.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoTabShift.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoTextBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JTEUndoTyping.h
# End Source File
# Begin Source File

SOURCE=.\code\JTextChooseSaveFile.h
# End Source File
# Begin Source File

SOURCE=.\code\JTextEditor.h
# End Source File
# Begin Source File

SOURCE=.\code\JTextProgressDisplay.h
# End Source File
# Begin Source File

SOURCE=.\code\JTextUserNotification.h
# End Source File
# Begin Source File

SOURCE=.\code\JThisProcess.h
# End Source File
# Begin Source File

SOURCE=.\code\jTime.h
# End Source File
# Begin Source File

SOURCE=.\code\JTree.h
# End Source File
# Begin Source File

SOURCE=.\code\JTreeList.h
# End Source File
# Begin Source File

SOURCE=.\code\JTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\code\jTypes.h
# End Source File
# Begin Source File

SOURCE=.\code\JUndo.h
# End Source File
# Begin Source File

SOURCE=.\code\JUserNotification.h
# End Source File
# Begin Source File

SOURCE=.\code\JValueTableData.h
# End Source File
# Begin Source File

SOURCE=.\code\JVector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPM.h
# End Source File
# End Group
# Begin Group "Templates"

# PROP Default_Filter "tmpl;tmpls"
# Begin Source File

SOURCE=.\code\JAliasArray.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JArray.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JAuxTableData.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JConstHashCursor.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JDynamicHistogram.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JHashCursor.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JHashRecord.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JHashTable.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JHistogram.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JLinkedList.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JLinkedListIterator.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JMinMax.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JObjTableData.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSet.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JOrderedSetIterator.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JOutPipeStreambuf.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JPackedTableData.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JPtrArray.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JPtrArrayIterator.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JPtrQueue.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JPtrStack.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JQueue.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JRunArray.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JRunArrayIterator.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JStack.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JStringMap.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JStringMapCursor.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JStringPtrMap.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JStringPtrMapCursor.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JStrValue.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JTaskIterator.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JValueTableData.tmpl
# End Source File
# End Group
# Begin Source File

SOURCE=.\code\JHTMLScannerL.l
# End Source File
# End Target
# End Project
