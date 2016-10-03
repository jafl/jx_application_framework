/******************************************************************************
 GNBHelpText.h

	Copyright (C) 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBHelpText
#define _H_GNBHelpText

#include <jTypes.h>

static const JCharacter* kTOCHelpName				= "GNBTOCHelp";
static const JCharacter* kMainHelpName				= "GNBMainHelp";
static const JCharacter* kNotesHelpName				= "GNBMainHelp#Notes";
static const JCharacter* kToDoHelpName				= "GNBMainHelp#ToDo";
static const JCharacter* kNoteEditorHelpName		= "GNBNoteEditorHelp";
static const JCharacter* kTrashHelpName				= "GNBTrashHelp";
static const JCharacter* kGettingStartedHelpName	= "GNBGettingStartedHelp";
static const JCharacter* kMiscHelpName				= "GNBMiscHelp";
static const JCharacter* kWMHelpName				= "GNBMainHelp#WM";
static const JCharacter* kChangeLogName				= "GNBChanges";
static const JCharacter* kCreditsName				= "GNBCredits";

static const JCharacter* kHelpSectionName[] =
	{ 
	kMainHelpName,
	kNotesHelpName,
	kToDoHelpName,
	kNoteEditorHelpName, 
	kTrashHelpName,
	kGettingStartedHelpName, 
	kChangeLogName, 
	kCreditsName,
	kMiscHelpName,
	kWMHelpName,
	kTOCHelpName
	};

static JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);

#endif
