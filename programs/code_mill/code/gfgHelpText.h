/******************************************************************************
 gfgHelpText.h

   Copyright (C) 2002 by New Planet Software. All rights reserved.

 ******************************************************************************/

#ifndef _H_gfgHelpText
#define _H_gfgHelpText

#include <jTypes.h>

static const JCharacter* kGFGTOCHelpName		= "GFGTOCHelp";
static const JCharacter* kGFGMainHelpName		= "GFGMainHelp";
static const JCharacter* kGFGOverviewHelpName	= "GFGOverviewHelp";
static const JCharacter* kGFGChangeLogName		= "GFGChanges";
static const JCharacter* kGFGCreditsName		= "GFGCredits";

static const JCharacter* kGFGHelpSectionName[] =
	{
	kGFGTOCHelpName,
	kGFGOverviewHelpName,
	kGFGMainHelpName,
	kGFGChangeLogName,
	kGFGCreditsName,
	"LICENSE"
	};

const JSize kGFGHelpSectionCount = sizeof(kGFGHelpSectionName) / sizeof(JCharacter*);

#endif
