/******************************************************************************
 JSTUndoTabShift.cpp

	Class to undo indenting a block of text.  We have to use JSTUndoPaste
	to get back the original pattern of spaces and tabs.

	BASE CLASS = JSTUndoPaste

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JSTUndoTabShift.h>
#include <JStyledText.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Saves a snapshot of the specified range, to allow undo.

 ******************************************************************************/

JSTUndoTabShift::JSTUndoTabShift
	(
	JStyledText*					text,
	const JStyledText::TextRange&	range
	)
	:
	JSTUndoPaste(text, range)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSTUndoTabShift::~JSTUndoTabShift()
{
}
