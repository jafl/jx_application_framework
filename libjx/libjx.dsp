# Microsoft Developer Studio Project File - Name="libjx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libjx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libjx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libjx.mak" CFG="libjx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libjx - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libjx - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libjx - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"JXStdInc.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libjx - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /Gy /I "code" /I "image" /I "..\libjcore\code" /I "..\misc\win32" /I "..\misc\win32\X" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D SIZEOF_LONG=4 /D SIZEOF_INT=4 /D "PCRE_STATIC" /D "ACE_HAS_ACE_TOKEN" /D "ACE_HAS_ACE_SVCCONF" /D "ACE_AS_STATIC_LIBS" /FR /Yu"JXStdInc.h" /FD /GZ /c
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

# Name "libjx - Win32 Release"
# Name "libjx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc;th"
# Begin Source File

SOURCE=.\code\JXAcceptLicenseDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jXActions.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXAdjustIWBoundsTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXAdjustScrollbarTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXAdjustToolBarGeometryTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXAnimationTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXAssert.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXAtLeastOneCBGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXBorderRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXButtonStates.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCardFile.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCharInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCheckboxGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCheckModTimeTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseColorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseFontSizeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseMonoFont.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXChoosePathDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseSaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCloseDirectorTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXColHeaderWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXColormap.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXComposeRuleList.cpp

!IF  "$(CFG)" == "libjx - Win32 Release"

!ELSEIF  "$(CFG)" == "libjx - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JXComposeScanner.cpp

!IF  "$(CFG)" == "libjx - Win32 Release"

!ELSEIF  "$(CFG)" == "libjx - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JXContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCreatePG.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCSFDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCSFSelectPrevDirTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCurrentPathMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCursorAnimationTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXCursorAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDecorRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDialogDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDirectSaveAction.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDirectSaveSource.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDirTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDisplayMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDNDChooseDropActionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDNDManager.cpp

!IF  "$(CFG)" == "libjx - Win32 Release"

!ELSEIF  "$(CFG)" == "libjx - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JXDNDManager_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDockDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDockDragData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDockManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDocktab.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDockWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDockWindowTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDocumentManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDocumentMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDownRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDragPainter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDSSFinishSaveTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXDSSSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXEditTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXEditWWWPrefsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXEmbossedRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXEngravedRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXEPSPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXEPSPrintSetupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXErrorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jXEventUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileHistoryMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileListSet.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileListTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileNameDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFileSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFixLenPGDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFlatRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFLInputBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFloatInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFloatTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFLRegexInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFLWildcardInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFontCharSetMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFontManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFontNameMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXFontSizeMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXGC.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXGetCurrColormap.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXGetCurrFontMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXGetNewDirDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXGetPreallocColor.th
# End Source File
# Begin Source File

SOURCE=.\code\jXGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXGoToLineDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHelpDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHelpManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHelpText.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHintDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHintManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHistoryMenuBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHorizDockPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXHorizPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXIconDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXIdleTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImage.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageCheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenuData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenuDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenuTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImagePainter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXImageWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXInputField.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXIntegerInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXKeyModifiers.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXLevelControl.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXLinkText.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMacWinPrefsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMDIServer.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuItemIDUtil.th
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXMessageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXNamedTreeListWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXNewDirButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXNewDisplayDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXOKToCloseDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jXPainterUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPasswordInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPathHistoryMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPathInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPGDirectorBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPGMessageDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPrefsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXProgressDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXProgressIndicator.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXProgressTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPSPageSetupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPSPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPSPrintSetupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPTPageSetupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPTPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXPTPrintSetupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXQuitIfAllDeactTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXRadioGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXRadioGroupDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXRegexInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXRegexReplaceInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXRowHeaderWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSaveFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSaveFileInput.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXScrollableWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXScrollbarSet.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXScrolltab.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSearchTextButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSearchTextDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSelectionData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSelectionManager.cpp

!IF  "$(CFG)" == "libjx - Win32 Release"

!ELSEIF  "$(CFG)" == "libjx - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JXSelectionManager_Win32.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSelectTabTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSharedPrefsManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSliderBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXSplashWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStandAlonePG.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStaticText.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStdInc.cpp
# ADD CPP /Yc"JXStdInc.h"
# End Source File
# Begin Source File

SOURCE=.\code\JXStringCompletionMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStringHistoryMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStringList.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStringTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleMenuDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleMenuTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleTableMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTabGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTEBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTEBlinkCaretTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTEStyleMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextCheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextEditorSet.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenuData.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenuDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenuTable.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTextSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTimerTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTipOfTheDayDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTLWAdjustToTreeTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarEditDir.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarEditWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarNode.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXTreeListWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXUNDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXUpdateDocMenuTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXUpdateShortcutIndex.th
# End Source File
# Begin Source File

SOURCE=.\code\JXUpdateWDMenuTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXUpRect.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXUrgentTask.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXUserNotification.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jXUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXVarLenPGDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXVertDockPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXVertPartition.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWarningDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWDManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWDMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWebBrowser.cpp

!IF  "$(CFG)" == "libjx - Win32 Release"

!ELSEIF  "$(CFG)" == "libjx - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\JXWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWidgetSet.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWindowDirector.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWindowIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\code\JXWindowPainter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\jXWinMain.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\code\JXXFontMenu.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\code\JXAcceptLicenseDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\jXActionDefs.h
# End Source File
# Begin Source File

SOURCE=.\code\jXActions.h
# End Source File
# Begin Source File

SOURCE=.\code\JXAdjustIWBoundsTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXAdjustScrollbarTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXAdjustToolBarGeometryTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXAnimationTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXApplication.h
# End Source File
# Begin Source File

SOURCE=.\code\JXAssert.h
# End Source File
# Begin Source File

SOURCE=.\code\JXAtLeastOneCBGroup.h
# End Source File
# Begin Source File

SOURCE=.\code\JXBorderRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXButtonStates.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCardFile.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCharInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCheckbox.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCheckboxGroup.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCheckModTimeTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseColorDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseFontSizeDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseMonoFont.h
# End Source File
# Begin Source File

SOURCE=.\code\JXChoosePathDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXChooseSaveFile.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCloseDirectorTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXColHeaderWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXColormap.h
# End Source File
# Begin Source File

SOURCE=.\code\JXComposeRuleList.h
# End Source File
# Begin Source File

SOURCE=.\code\JXComposeScanner.h
# End Source File
# Begin Source File

SOURCE=.\code\jXConstants.h
# End Source File
# Begin Source File

SOURCE=.\code\JXContainer.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCreatePG.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCSFDialogBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCSFSelectPrevDirTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCurrentPathMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCursor.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCursorAnimationTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXCursorAnimator.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDecorRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDeleteObjectTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDialogDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDirectSaveAction.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDirectSaveSource.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDirTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDisplay.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDisplayMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDNDChooseDropActionDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDNDManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDockDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDockDragData.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDockManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDocktab.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDockWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDockWindowTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDocument.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDocumentManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDocumentMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDownRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDragPainter.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDSSFinishSaveTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXDSSSelection.h
# End Source File
# Begin Source File

SOURCE=.\code\JXEditTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXEditWWWPrefsDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXEmbossedRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXEngravedRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXEPSPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JXEPSPrintSetupDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXErrorDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\jXEventUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileDocument.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileHistoryMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileListSet.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileListTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileNameDisplay.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFileSelection.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFixLenPGDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFlatRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFLInputBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFloatInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFloatTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFLRegexInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFLWildcardInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFontCharSetMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFontManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFontNameMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXFontSizeMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXGC.h
# End Source File
# Begin Source File

SOURCE=.\code\JXGetCurrColormap.h
# End Source File
# Begin Source File

SOURCE=.\code\JXGetCurrFontMgr.h
# End Source File
# Begin Source File

SOURCE=.\code\JXGetNewDirDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\jXGlobals.h
# End Source File
# Begin Source File

SOURCE=.\code\JXGoToLineDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHelpDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHelpManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHelpText.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHintDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHintManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHistoryMenuBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHorizDockPartition.h
# End Source File
# Begin Source File

SOURCE=.\code\JXHorizPartition.h
# End Source File
# Begin Source File

SOURCE=.\code\JXIconDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXIdleTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImage.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageCheckbox.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenuData.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenuDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageMenuTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImagePainter.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageSelection.h
# End Source File
# Begin Source File

SOURCE=.\code\JXImageWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXInputField.h
# End Source File
# Begin Source File

SOURCE=.\code\JXIntegerInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXKeyModifiers.h
# End Source File
# Begin Source File

SOURCE=.\code\jXKeysym.h
# End Source File
# Begin Source File

SOURCE=.\code\JXLevelControl.h
# End Source File
# Begin Source File

SOURCE=.\code\JXLibVersion.h
# End Source File
# Begin Source File

SOURCE=.\code\JXLinkText.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMacWinPrefsDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMDIServer.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuBar.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuData.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMenuTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXMessageDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXNamedTreeListWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXNewDirButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXNewDisplayDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXOKToCloseDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\jXPainterUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPartition.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPasswordInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPathHistoryMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPathInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPGDirectorBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPGMessageDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPrefsManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXProgressDisplay.h
# End Source File
# Begin Source File

SOURCE=.\code\JXProgressIndicator.h
# End Source File
# Begin Source File

SOURCE=.\code\JXProgressTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPSPageSetupDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPSPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPSPrintSetupDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPTPageSetupDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPTPrinter.h
# End Source File
# Begin Source File

SOURCE=.\code\JXPTPrintSetupDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXQuitIfAllDeactTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXRadioGroup.h
# End Source File
# Begin Source File

SOURCE=.\code\JXRadioGroupDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXRegexInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXRegexReplaceInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXRowHeaderWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSaveFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSaveFileInput.h
# End Source File
# Begin Source File

SOURCE=.\code\JXScrollableWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\code\JXScrollbarSet.h
# End Source File
# Begin Source File

SOURCE=.\code\JXScrolltab.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSearchTextButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSearchTextDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSelectionData.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSelectionManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSelectTabTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSharedPrefsManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSlider.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSliderBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXSplashWindow.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStandAlonePG.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStaticText.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStringCompletionMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStringHistoryMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStringList.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStringTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleMenuDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleMenuTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXStyleTableMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTabGroup.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTEBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTEBlinkCaretTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTEStyleMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextCheckbox.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextEditor.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextEditorSet.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenuData.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenuDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextMenuTable.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTextSelection.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTimerTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTipOfTheDayDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTLWAdjustToTreeTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBar.h
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarButton.h
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarEditDir.h
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarEditWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXToolBarNode.h
# End Source File
# Begin Source File

SOURCE=.\code\JXTreeListWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXUNDialogBase.h
# End Source File
# Begin Source File

SOURCE=.\code\JXUpdateDocMenuTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXUpdateWDMenuTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXUpRect.h
# End Source File
# Begin Source File

SOURCE=.\code\JXUrgentTask.h
# End Source File
# Begin Source File

SOURCE=.\code\JXUserNotification.h
# End Source File
# Begin Source File

SOURCE=.\code\jXUtil.h
# End Source File
# Begin Source File

SOURCE=.\code\JXVarLenPGDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXVertDockPartition.h
# End Source File
# Begin Source File

SOURCE=.\code\JXVertPartition.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWarningDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWDManager.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWDMenu.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWebBrowser.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWidget.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWidgetSet.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWindow.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWindowDirector.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWindowIcon.h
# End Source File
# Begin Source File

SOURCE=.\code\JXWindowPainter.h
# End Source File
# Begin Source File

SOURCE=.\code\JXXFontMenu.h
# End Source File
# End Group
# Begin Group "Templates"

# PROP Default_Filter "tmpl;tmpls"
# Begin Source File

SOURCE=.\code\JXDeleteObjectTask.tmpl
# End Source File
# Begin Source File

SOURCE=.\code\JXDeleteObjectTask.tmpls
# End Source File
# End Group
# Begin Group "Images"

# PROP Default_Filter "xpm"
# Begin Source File

SOURCE=.\image\jx_binary_file_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_binary_file_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_cdrom_disk_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_cdrom_disk_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_cdrom_disk_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_check_spelling.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_down_chevron.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_clean_right_margin.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_clear.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_coerce_right_margin.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_copy.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_cut.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_force_shift_left.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_paste.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_read_only.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_redo.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_shift_left.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_shift_right.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_edit_undo.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_executable_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_executable_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_file_new.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_file_open.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_file_print.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_file_revert_to_saved.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_file_save.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_file_save_all.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_filter_regex.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_filter_wildcard.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_find.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_find_next.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_find_previous.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_find_selection_backwards.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_find_selection_forward.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_floppy_disk_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_floppy_disk_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_floppy_disk_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_locked_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_locked_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_read_only_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_read_only_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_read_only_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_read_only_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_folder_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_hard_disk_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_hard_disk_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_hard_disk_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_help_specific.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_help_toc.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_home_folder_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_home_folder_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_image_file_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_image_file_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_library_file_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_library_file_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_plain_file_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_plain_file_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_replace_selection.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_run_script.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_source_file_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_source_file_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_tip_of_the_day.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_toolbar_checked.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_toolbar_menu_node.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_toolbar_unchecked.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_empty_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_empty_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_empty_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_empty_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_full_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_full_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_full_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_trash_can_full_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_un_error.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_un_message.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_un_warning.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_unknown_file_selected_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_unknown_file_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_up_chevron.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_zip_disk_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_zip_disk_selected_large.xpm
# End Source File
# Begin Source File

SOURCE=.\image\jx_zip_disk_small.xpm
# End Source File
# Begin Source File

SOURCE=.\image\new_planet_software.xpm
# End Source File
# Begin Source File

SOURCE=.\image\new_planet_software_small.xpm
# End Source File
# End Group
# Begin Source File

SOURCE=.\code\JXComposeScannerL.l
# End Source File
# End Target
# End Project
