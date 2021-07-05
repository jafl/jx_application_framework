/******************************************************************************
 cbActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_cbActionDefs
#define _H_cbActionDefs

#include <jXActionDefs.h>	// for convenience

// File menu

#define kCBNewTextFileAction         "CBNewTextFile"
#define kCBNewTextFileFromTmplAction "CBNewTextFileFromTmpl"
#define kCBNewProjectAction          "CBNewProject"
#define kCBNewShellAction            "CBNewShell"
#define kCBOpenSomethingAction       "CBOpenSomething"
#define kCBSaveFileAction            "CBSaveFile"
#define kCBSaveFileAsAction          "CBSaveFileAs"
#define kCBSaveCopyAsAction          "CBSaveCopyAs"
#define kCBSaveAsTemplateAction      "CBSaveAsTemplate"
#define kCBRevertAction              "CBRevert"
#define kCBSaveAllTextFilesAction    "CBSaveAllTextFiles"
#define kCBPageSetupStyledTextAction "CBPageSetupStyledText"
#define kCBPrintStyledTextAction     "CBPrintStyledText"
#define kCBCloseAllTextFilesAction   "CBCloseAllTextFiles"
#define kCBShowInFileMgrAction       "CBShowInFileMgr"

#define kCBDiffFilesAction "CBDiffFiles"
#define kCBDiffSmartAction "CBDiffSmart"
#define kCBDiffVCSAction   "CBDiffVCS"

#define kCBUNIXTextFormatAction "CBUNIXTextFormat"
#define kCBMacTextFormatAction  "CBMacTextFormat"
#define kCBDOSTextFormatAction  "CBDOSTextFormat"

// Project menu

#define kCBMakeBinaryAction       "CBMakeBinary"
#define kCBCompileFileAction      "CBCompileFile"
#define kCBEditMakeSettingsAction "CBEditMakeSettings"
#define kCBRunProgramAction       "CBRunProgram"
#define kCBDebugFileAction        "CBDebugFile"
#define kCBEditRunSettingsAction  "CBEditRunSettings"

#define kCBAddToProjAbsoluteAction     "CBAddToProjAbsolute"
#define kCBAddToProjProjRelativeAction "CBAddToProjProjRelative"
#define kCBAddToProjHomeRelativeAction "CBAddToProjHomeRelative"

#define kCBNewProjectGroupAction           "CBNewProjectGroup"
#define kCBAddFilesToProjectAction         "CBAddFilesToProject"
#define kCBAddDirectoryTreeToProjectAction "CBAddDirectoryTreeToProject"
#define kCBOpenSelectedFilesAction         "CBOpenSelectedFiles"
#define kCBOpenSelectedFileLocationsAction "CBOpenSelectedFileLocations"
#define kCBOpenComplFilesAction            "CBOpenComplFiles"
#define kCBRemoveFilesAction               "CBRemoveFiles"
#define kCBEditFilePathAction              "CBEditFilePath"
#define kCBEditSubprojConfigAction         "CBEditSubprojConfig"
#define kCBEditMakeConfigAction            "CBEditMakeConfig"
#define kCBUpdateMakefileAction            "CBUpdateMakefile"
#define kCBShowSymbolBrowserAction         "CBShowSymbolBrowser"
#define kCBShowCPPClassTreeAction          "CBShowCPPClassTree"
#define kCBShowDClassTreeAction            "CBShowDClassTree"
#define kCBShowGoClassTreeAction           "CBShowGoClassTree"
#define kCBShowJavaClassTreeAction         "CBShowJavaClassTree"
#define kCBShowPHPClassTreeAction          "CBShowPHPClassTree"
#define kCBViewManPageAction               "CBViewManPage"
#define kCBEditSearchPathsAction           "CBEditSearchPaths"
#define kCBShowFileListAction              "CBShowFileList"
#define kCBFindFileAction                  "CBFindFile"
#define kCBSearchFilesAction               "CBSearchFiles"

// Search menu

#define kCBBalanceGroupingSymbolAction  "CBBalanceGroupingSymbol"
#define kCBPlaceBookmarkAction          "CBPlaceBookmark"
#define kCBGoToLineAction               "CBGoToLine"
#define kCBGoToColumnAction             "CBGoToColumn"
#define kCBOpenSelectionAsFileAction    "CBOpenSelectionAsFile"
#define kCBFindSelectionInProjectAction "CBFindSelectionInProject"
#define kCBFindSelInProjNoContextAction "CBFindSelectionInProjectNoContext"
#define kCBFindSelectionInManPageAction "CBFindSelectionInManPage"
#define kCBFindFileAction               "CBFindFile"
#define kCBOpenManPageAction            "CBOpenManPage"
#define kCBOpenPrevListItem				"CBOpenPrevListItem"
#define kCBOpenNextListItem				"CBOpenNextListItem"

// Symbol menu

#define kCBCopySymbolNameAction "CBCopySymbolName"
#define kCBCloseAllSymSRAction  "CBCloseAllSymSR"

// Tree menus

#define kCBPrintTreeEPSAction "CBPrintTreeEPS"

#define kCBConfigureCTreeCPPAction     "CBConfigureCTreeCPP"
#define kCBUpdateClassTreeAction       "CBUpdateClassTree"
#define kCBMinimizeMILinkLengthAction  "CBMinimizeMILinkLength"
#define kCBOpenClassFnListAction       "CBOpenClassFnList"
#define kCBCollapseClassesAction       "CBCollapseClasses"
#define kCBExpandClassesAction         "CBExpandClasses"
#define kCBExpandAllClassesAction      "CBExpandAllClasses"
#define kCBSelectParentClassAction     "CBSelectParentClass"
#define kCBSelectDescendantClassAction "CBSelectDescendantClass"
#define kCBCopyClassNameAction         "CBCopyClassName"
#define kCBCreateDerivedClassAction    "CBCreateDerivedClass"
#define kCBFindFunctionAction          "CBFindFunction"

// File List menu

#define kCBUseWildcardFilterAction "CBUseWildcardFilter"
#define kCBUseRegexFilterAction    "CBUseRegexFilter"

#endif
