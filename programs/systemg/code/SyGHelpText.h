/******************************************************************************
 SyGHelpText.h

	Copyright (C) 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGHelpText
#define _H_SyGHelpText

#include <jTypes.h>

static const JCharacter* kSyGTOCHelpName			= "SyGTOCHelp";
static const JCharacter* kSyGGettingStartedHelpName	= "SyGGettingStartedHelp";

static const JCharacter* kSyGFileHelpName			= "SyGFileHelp";
static const JCharacter* kSyGFolderHelpName			= "SyGFolderHelp";
static const JCharacter* kSyGExecHelpName			= "SyGExecHelp";
static const JCharacter* kSyGGitHelpName			= "SyGGitHelp";
static const JCharacter* kSyGBindingEditorHelpName	= "JFSBindingEditorHelp";
static const JCharacter* kSyGRunCommandHelpName		= "JFSRunCommandHelp";
static const JCharacter* kSyGRunFileHelpName		= "JFSRunFileHelp";

static const JCharacter* kSyGSearchTextHelpName		= "JXSearchTextHelp";
static const JCharacter* kSyGRegexHelpName			= "JXRegexHelp";
static const JCharacter* kSyGRegexReplaceHelpName	= "JXRegexHelp#Replace";
static const JCharacter* kSyGRegexQRefName			= "JXRegexQRef";

static const JCharacter* kSyGMiscHelpName			= "SyGMiscHelp";
static const JCharacter* kSyGManPageHelpName		= "SyGMiscHelp#ViewMan";

static const JCharacter* kSyGChangeLogName			= "SyGChanges";
static const JCharacter* kSyGCreditsName			= "SyGCredits";

static const JCharacter* kSyGHelpSectionName[] =
	{
	kSyGTOCHelpName,
	kSyGGettingStartedHelpName,
	kSyGFileHelpName,
	kSyGFolderHelpName,
	kSyGExecHelpName,
	kSyGGitHelpName,
	kSyGBindingEditorHelpName,
	kSyGRunCommandHelpName,
	kSyGRunFileHelpName,
	kSyGSearchTextHelpName,
	kSyGRegexHelpName,
	kSyGRegexQRefName,
	kSyGMiscHelpName,
	kSyGChangeLogName,
	kSyGCreditsName,
	"LICENSE"
	};

static JSize kSyGHelpSectionCount = sizeof(kSyGHelpSectionName) / sizeof(JCharacter*);

#endif
