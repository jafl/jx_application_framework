/******************************************************************************
 JTEUndoTabShift.cpp

	Class to undo indenting a block of text.  We have to use JTEUndoPaste
	to get back the original pattern of spaces and tabs.

	BASE CLASS = JTEUndoPaste

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#include <JTEUndoTabShift.h>
#include <JTextEditor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTabShift::JTEUndoTabShift
	(
	JTextEditor* te
	)
	:
	JTEUndoPaste(te, JTextEditor::TextCount(0,0))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTabShift::~JTEUndoTabShift()
{
}

/******************************************************************************
 UpdateEndChar

	Call this after SetSelection() to update the end of the affected text.
	(We can't store a line number, because this may change when word
	 wrapping is on.)

 ******************************************************************************/

void
JTEUndoTabShift::UpdateEndChar()
{
	Activate();		// cancel SetSelection()

	JCharacterRange cr;
	JUtf8ByteRange  br;
	const JBoolean ok = GetTE()->GetSelection(&cr, &br);
	assert( ok );
	SetCurrentCount(JTextEditor::TextCount(cr.GetCount(), br.GetCount()));
}
