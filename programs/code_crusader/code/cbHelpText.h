/******************************************************************************
 cbHelpText.h

	Copyright © 1997-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_cbHelpText
#define _H_cbHelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

static const JCharacter* kCBTOCHelpName      = "CBTOCHelp";
static const JCharacter* kCBOverviewHelpName = "CBOverviewHelp";

static const JCharacter* kCBEditorHelpName                = "CBEditorHelp";
static const JCharacter* kCBEditorDNDHelpName             = "CBEditorHelp#DND";
static const JCharacter* kCBEditorBalanceGroupingHelpName = "CBEditorHelp#BalanceGrouping";
static const JCharacter* kCBEditorSourceCodeHelpName      = "CBEditorHelp#SourceCode";
static const JCharacter* kCBEditorHTMLHelpName            = "CBEditorHelp#HTML";
static const JCharacter* kCBEditorMouseShortcutsHelpName  = "CBEditorHelp#MouseShortcuts";
static const JCharacter* kCBEditorKeyShortcutsHelpName    = "CBEditorHelp#KeyShortcuts";
static const JCharacter* kCBEditorExtScriptHelpName       = "CBEditorHelp#ExtScript";
static const JCharacter* kCBEditorPrefsHelpName           = "CBEditorHelp#Prefs";

static const JCharacter* kCBSearchTextHelpName   = "CBSearchTextHelp";
static const JCharacter* kCBSearchFilesHelpName  = "CBSearchTextHelp#Multifile";

static const JCharacter* kCBRegexHelpName        = "JXRegexHelp";
static const JCharacter* kCBRegexReplaceHelpName = "JXRegexHelp#Replace";

static const JCharacter* kCBRegexQRefName        = "JXRegexQRef";

static const JCharacter* kCBCompleterHelpName = "CBCompleterHelp";
static const JCharacter* kCBFileTypeHelpName  = "CBFileTypeHelp";
static const JCharacter* kCBMacroHelpName     = "CBMacroHelp";
static const JCharacter* kCBStylerHelpName    = "CBStylerHelp";
static const JCharacter* kCBCRMHelpName       = "CBCRMHelp";

static const JCharacter* kCBProjectHelpName          = "CBProjectHelp";
static const JCharacter* kCBProjectConfigHelpName    = "CBProjectHelp#Config";
static const JCharacter* kCBProjectPrefsHelpName     = "CBProjectHelp#Prefs";
static const JCharacter* kCBExtEditorHelpName        = "CBProjectHelp#ExtEditor";

static const JCharacter* kCBProjectTutorialHelpName  = "CBProjectTutorialHelp";
static const JCharacter* kCBMakemakeHelpName         = "CBMakemakeHelp";

static const JCharacter* kCBSymbolHelpName           = "CBSymbolHelp";
static const JCharacter* kCBSymbolPrefsHelpName      = "CBSymbolHelp#Prefs";

static const JCharacter* kCBFileListHelpName         = "CBFileListHelp";

static const JCharacter* kCBCTreeHelpName            = "CBCTreeHelp";
static const JCharacter* kCBCFunctionListHelpName    = "CBCTreeHelp#FunctionList";
static const JCharacter* kCBCTreeCPPHelpName         = "CBCTreeHelp#CPP";

static const JCharacter* kCBJavaTreeHelpName         = "CBJavaTreeHelp";
static const JCharacter* kCBJavaFunctionListHelpName = "CBJavaTreeHelp#FunctionList";

static const JCharacter* kCBTreePrefsHelpName        = "CBTreePrefsHelp";

static const JCharacter* kCBTasksHelpName            = "CBTasksHelp";
static const JCharacter* kCBCompileHelpName          = "CBCompileHelp";
static const JCharacter* kCBRunProgramHelpName       = "CBRunProgramHelp";
static const JCharacter* kCBShellHelpName            = "CBShellHelp";
static const JCharacter* kCBFileTemplateHelpName     = "CBFileTemplateHelp";

static const JCharacter* kCBManPageHelpName          = "CBManPageHelp";
static const JCharacter* kCBDiffHelpName             = "CBDiffHelp";

static const JCharacter* kCBMiscHelpName             = "CBMiscHelp";
static const JCharacter* kCBMiscShortcutsHelpName    = "CBMiscHelp#Shortcuts";
static const JCharacter* kCBScrollingHelpName        = "CBMiscHelp#Scrolling";
static const JCharacter* kCBFileChooserHelpName      = "CBMiscHelp#FileChooser";
static const JCharacter* kCBMultiSelectHelpName      = "CBMiscHelp#MultiSelect";
static const JCharacter* kCBColorChooserHelpName     = "CBMiscHelp#ColorChooser";
static const JCharacter* kCBWindowManagementHelpName = "CBMiscHelp#WindowManagement";
static const JCharacter* kCBModifierKeysHelpName     = "CBMiscHelp#ModifierKeys";

static const JCharacter* kCBDockHelpName  = "JXDockHelp";

static const JCharacter* kCBChangeLogName = "CBChangeLog";
static const JCharacter* kCBCreditsName   = "CBCredits";

static const JCharacter* kCBHelpSectionName[] =
	{
	kCBTOCHelpName, kCBOverviewHelpName,
	kCBEditorHelpName,
	kCBSearchTextHelpName,
	kCBRegexHelpName, kCBRegexQRefName,
	kCBCompleterHelpName,
	kCBFileTypeHelpName, kCBMacroHelpName, kCBStylerHelpName,
	kCBCRMHelpName,
	kCBProjectHelpName, kCBProjectTutorialHelpName, kCBMakemakeHelpName,
	kCBSymbolHelpName, kCBFileListHelpName,
	kCBCTreeHelpName, kCBJavaTreeHelpName, kCBTreePrefsHelpName,
	kCBTasksHelpName, kCBCompileHelpName, kCBRunProgramHelpName, kCBShellHelpName,
	kCBFileTemplateHelpName,
	kCBManPageHelpName, kCBDiffHelpName,
	kCBMiscHelpName, kCBDockHelpName, kCBChangeLogName, kCBCreditsName,
	"LICENSE"
	};

const JSize kCBHelpSectionCount = sizeof(kCBHelpSectionName) / sizeof(JCharacter*);

#endif
