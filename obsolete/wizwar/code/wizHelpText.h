/******************************************************************************
 wizHelpText.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_wizHelpText
#define _H_wizHelpText

#include <jTypes.h>

static const JCharacter* kWizTOCHelpName      = "WizTOCHelp";
static const JCharacter* kWizOverviewHelpName = "WizOverviewHelp";
static const JCharacter* kWizConnectHelpName  = "WizConnectHelp";
static const JCharacter* kWizChatHelpName     = "WizChatHelp";
static const JCharacter* kWizChangeLogName    = "WizChangeLog";
static const JCharacter* kWizCreditsName      = "WizCredits";

static const JCharacter* kWizHelpSectionName[] =
	{
	kWizTOCHelpName, kWizOverviewHelpName,
	kWizConnectHelpName, kWizChatHelpName,
	kWizChangeLogName, kWizCreditsName
	};

const JSize kWizHelpSectionCount = sizeof(kWizHelpSectionName) / sizeof(JCharacter*);

#endif
