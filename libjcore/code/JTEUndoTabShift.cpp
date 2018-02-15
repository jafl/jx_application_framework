/******************************************************************************
 JTEUndoTabShift.cpp

	Class to undo indenting a block of text.  We have to use JTEUndoPaste
	to get back the original pattern of spaces and tabs.

	BASE CLASS = JTEUndoPaste

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTabShift.h>
#include <JStyledTextBuffer.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTabShift::JTEUndoTabShift
	(
	JStyledTextBuffer* te
	)
	:
	JTEUndoPaste(te, JStyledTextBuffer::TextCount(0,0))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTabShift::~JTEUndoTabShift()
{
}
