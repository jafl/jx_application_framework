/******************************************************************************
 GNBHelpText.cc

	Copyright © 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GNBHelpText.h"

const JCharacter* kHelpSectionName[] =
	{ kMainHelpName, kNoteEditorHelpName, kTrashHelpName,
		kGettingStartedHelpName, kChangeLogName, kCreditsName,
		kMiscHelpName, kTOCHelpName};

const JCharacter* kHelpSectionTitle[] =
	{ kMainHelpTitle, kNoteEditorHelpTitle, kTrashHelpTitle,
		kGettingStartedHelpTitle, kChangeLogTitle, kCreditsTitle,
		kMiscHelpTitle, kTOCHelpTitle };

const JCharacter* kHelpSectionText[] =
	{ kMainHelpText, kNoteEditorHelpText, kTrashHelpText,
		kGettingStartedHelpText, kChangeLogText, kCreditsText,
		kMiscHelpText, kTOCHelpText };

JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);
