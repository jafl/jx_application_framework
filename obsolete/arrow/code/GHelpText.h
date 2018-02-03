/******************************************************************************
 GHelpText.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GHelpText
#define _H_GHelpText

#include <jTypes.h>

static const JCharacter* kGMailboxHelpName		= "GMailboxHelp";
static const JCharacter* kGAccountHelpName		= "GAccountHelp";
static const JCharacter* kGFilterHelpName		= "GFilterHelp";
static const JCharacter* kGSendHelpName			= "GSendHelp";
static const JCharacter* kGViewHelpName			= "GViewHelp";
static const JCharacter* kGOverviewHelpName		= "GOverviewHelp";
static const JCharacter* kGABHelpName			= "GABHelp";
static const JCharacter* kGMailTreeHelpName		= "GMailTreeHelp";
static const JCharacter* kGChangeLogName		= "GChangeLog";
static const JCharacter* kGCreditsName			= "GCredits";
static const JCharacter* kGPOPHelpName			= "GPOPHelp";
static const JCharacter* kGTOCHelpName			= "GTOCHelp";
static const JCharacter* kGPrefsHelpName		= "GPrefsHelp";

static const JCharacter* kHelpSectionName[] =
{
	kGMailboxHelpName,
	kGAccountHelpName,
	kGFilterHelpName,
	kGSendHelpName,
	kGViewHelpName,
	kGOverviewHelpName,
	kGABHelpName,
	kGMailTreeHelpName,
	kGChangeLogName,
	kGCreditsName,
	kGPOPHelpName,
	kGTOCHelpName,
	kGPrefsHelpName,
	"LICENSE"
};

static JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);

#endif
