/******************************************************************************
 scHelpText.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_scHelpText
#define _H_scHelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

static const JCharacter* kSCMiscHelpName  = "SCMiscHelp";
static const JCharacter* kSCExprHelpName  = "SCExprHelp";
static const JCharacter* kSCChangeLogName = "SCChangeLog";
static const JCharacter* kSCCreditsName   = "SCCredits";

static const JCharacter* kSCHelpSectionName[] =
	{
	kSCMiscHelpName, kSCExprHelpName,
	kSCChangeLogName, kSCCreditsName
	};

const JSize kSCHelpSectionCount = sizeof(kSCHelpSectionName) / sizeof(JCharacter*);

#endif
