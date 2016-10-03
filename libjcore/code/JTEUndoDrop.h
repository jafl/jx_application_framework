/******************************************************************************
 JTEUndoDrop.h

	Interface for the JTEUndoDrop class.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoDrop
#define _H_JTEUndoDrop

#include <JTEUndoBase.h>

class JTEUndoDrop : public JTEUndoBase
{
public:

	JTEUndoDrop(JTextEditor* te, const JIndex origIndex,
				const JIndex newIndex, const JSize length);

	virtual ~JTEUndoDrop();

	virtual void	SetPasteLength(const JSize length);
	virtual void	Undo();

private:

	JIndex	itsOrigCaretLoc;
	JIndex	itsNewSelStart;
	JIndex	itsNewSelEnd;

private:

	// not allowed

	JTEUndoDrop(const JTEUndoDrop& source);
	const JTEUndoDrop& operator=(const JTEUndoDrop& source);
};

#endif
