/******************************************************************************
 jxwHelpText.h

   Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jxwHelpText
#define _H_jxwHelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

static const JCharacter* kJXWTOCHelpName	= "JXWTOCHelp";
static const JCharacter* kJXWMainHelpName	= "JXWMainHelp";
static const JCharacter* kJXWChangeLogName	= "JXWChanges";
static const JCharacter* kJXWCreditsName	= "JXWCredits";

static const JCharacter* kJXWHelpSectionName[] =
	{
	kJXWTOCHelpName,
	kJXWMainHelpName,
	kJXWChangeLogName,
	kJXWCreditsName
	};

const JSize kJXWHelpSectionCount = sizeof(kJXWHelpSectionName) / sizeof(JCharacter*);

#endif
