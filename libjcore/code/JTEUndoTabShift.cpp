/******************************************************************************
 JTEUndoTabShift.cpp

	Class to undo indenting a block of text.  We have to use JTEUndoPaste
	to get back the original pattern of spaces and tabs.

	BASE CLASS = JTEUndoPaste

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTabShift.h>
#include <JStyledTextBuffer.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Saves a snapshot of the specified range, to allow undo.

 ******************************************************************************/

JTEUndoTabShift::JTEUndoTabShift
	(
	JStyledTextBuffer*					buffer,
	const JStyledTextBuffer::TextRange&	range
	)
	:
	JTEUndoPaste(buffer, range)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTabShift::~JTEUndoTabShift()
{
}
