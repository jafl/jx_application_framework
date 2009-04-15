/******************************************************************************
 testjxHelpText.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_testjxHelpText
#define _H_testjxHelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
