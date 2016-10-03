/******************************************************************************
 GHelpText.cc

	Copyright (C) 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GHelpText.h"

const JCharacter* kHelpSectionName[] =
	{ kGMailboxHelpName, kGAccountHelpName, kGFilterHelpName,
	  kGSendHelpName, kGViewHelpName,
	  kGOverviewHelpName, kGABHelpName, kGMailTreeHelpName,
	  kGChangeLogName, kGCreditsName, kGPOPHelpName, kGTOCHelpName,
	  kGPrefsHelpName };

JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);
