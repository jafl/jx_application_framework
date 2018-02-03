/******************************************************************************
 JTEUndoDrop.h

	Interface for the JTEUndoDrop class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoDrop
#define _H_JTEUndoDrop

#include <JTEUndoBase.h>

class JTEUndoDrop : public JTEUndoBase
{
public:

	JTEUndoDrop(JTextEditor* te,
				const JTextEditor::TextIndex& origIndex,
				const JTextEditor::TextIndex& newIndex,
				const JTextEditor::TextCount& length);

	virtual ~JTEUndoDrop();

	virtual void	SetPasteLength(const JTextEditor::TextCount& length);
	virtual void	Undo();

private:

	JTextEditor::TextIndex	itsOrigCaretLoc;
	JTextEditor::TextIndex	itsNewSelStart;
	JTextEditor::TextIndex	itsNewSelEnd;

private:

	// not allowed

	JTEUndoDrop(const JTEUndoDrop& source);
	const JTEUndoDrop& operator=(const JTEUndoDrop& source);
};

#endif
