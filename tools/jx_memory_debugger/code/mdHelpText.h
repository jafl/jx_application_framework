/******************************************************************************
 mdHelpText.h

   Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_mdHelpText
#define _H_mdHelpText

#include <jTypes.h>

static const JCharacter* kMDTOCHelpName			= "MDTOCHelp";
static const JCharacter* kMDOverviewHelpName	= "MDOverviewHelp";
static const JCharacter* kMDMainHelpName		= "MDMainHelp";
static const JCharacter* kMDRecordHelpName		= "MDRecordHelp";
static const JCharacter* kMDChangeLogName		= "MDChanges";
static const JCharacter* kMDCreditsName			= "MDCredits";

static const JCharacter* kMDHelpSectionName[] =
	{
	kMDTOCHelpName,
	kMDOverviewHelpName,
	kMDMainHelpName,
	kMDRecordHelpName,
	kMDChangeLogName,
	kMDCreditsName
	};

const JSize kMDHelpSectionCount = sizeof(kMDHelpSectionName) / sizeof(JCharacter*);

#endif
