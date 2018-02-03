/******************************************************************************
 jxwHelpText.h

   Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_jxwHelpText
#define _H_jxwHelpText

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
