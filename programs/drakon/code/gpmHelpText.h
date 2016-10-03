/******************************************************************************
 gpmHelpText.h

   Copyright (C) 2001 by New Planet Software. All rights reserved.

 ******************************************************************************/

#ifndef _H_gpmHelpText
#define _H_gpmHelpText

#include <jTypes.h>

static const JCharacter* kGPMTOCHelpName		= "GPMTOCHelp";
static const JCharacter* kGPMMainHelpName		= "GPMMainHelp";
static const JCharacter* kGPMOverviewHelpName	= "GPMOverviewHelp";
static const JCharacter* kGPMChangeLogName		= "GPMChanges";
static const JCharacter* kGPMCreditsName		= "GPMCredits";

static const JCharacter* kGPMHelpSectionName[] =
	{
	kGPMTOCHelpName,
	kGPMOverviewHelpName,
	kGPMMainHelpName,
	kGPMChangeLogName,
	kGPMCreditsName,
	"LICENSE"
	};

const JSize kGPMHelpSectionCount = sizeof(kGPMHelpSectionName) / sizeof(JCharacter*);

#endif
