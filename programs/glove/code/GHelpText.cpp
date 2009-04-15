/******************************************************************************
 GHelpText.cpp

	Copyright © 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GHelpText.h"

const JCharacter* kHelpSectionName[] =
	{ kOverviewHelpName };

const JCharacter* kHelpSectionTitle[] =
	{ kOverviewHelpTitle };

const JCharacter* kHelpSectionText[] =
	{ kOverviewHelpText };

JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);
