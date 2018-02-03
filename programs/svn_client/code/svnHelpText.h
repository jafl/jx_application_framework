/******************************************************************************
 svnHelpText.h

   Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_svnHelpText
#define _H_svnHelpText

#include <jTypes.h>

static const JCharacter* kSVNTOCHelpName      = "SVNTOCHelp";
static const JCharacter* kSVNOverviewHelpName = "SVNOverviewHelp";
static const JCharacter* kSVNMainHelpName     = "SVNMainHelp";
static const JCharacter* kSVNPrefsHelpName    = "SVNPrefsHelp";
static const JCharacter* kSVNChangeLogName    = "SVNChanges";
static const JCharacter* kSVNCreditsName      = "SVNCredits";

static const JCharacter* kSVNHelpSectionName[] =
	{
	kSVNTOCHelpName,
	kSVNOverviewHelpName,
	kSVNMainHelpName,
	kSVNPrefsHelpName,
	kSVNChangeLogName,
	kSVNCreditsName,
	"LICENSE"
	};

const JSize kSVNHelpSectionCount = sizeof(kSVNHelpSectionName) / sizeof(JCharacter*);

#endif
