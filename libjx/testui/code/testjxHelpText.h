/******************************************************************************
 testjxHelpText.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_testjxHelpText
#define _H_testjxHelpText

#include <jTypes.h>

static const JUtf8Byte* kMainHelpName  = "testjxMainHelp";
static const JUtf8Byte* kHyperHelpName = "testjxHyperHelp";
static const JUtf8Byte* kNoTextName    = "testjxNoTextHelp";

static const JUtf8Byte* kHelpSectionName[] =
	{
	kMainHelpName, kHyperHelpName, kNoTextName
	};

const JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JUtf8Byte*);

#endif
