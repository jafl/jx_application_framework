/******************************************************************************
 testjxHelpText.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_testjxHelpText
#define _H_testjxHelpText

#include <jTypes.h>

static const JCharacter* kMainHelpName  = "testjxMainHelp";
static const JCharacter* kHyperHelpName = "testjxHyperHelp";
static const JCharacter* kNoTextName    = "testjxNoTextHelp";

static const JCharacter* kHelpSectionName[] =
	{
	kMainHelpName, kHyperHelpName, kNoTextName
	};

const JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);

#endif
