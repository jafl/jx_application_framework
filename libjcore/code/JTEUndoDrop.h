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

	JTEUndoDrop(JStyledTextBuffer* te,
				const JStyledTextBuffer::TextIndex& origIndex,
				const JStyledTextBuffer::TextIndex& newIndex,
				const JStyledTextBuffer::TextCount& count);

	virtual ~JTEUndoDrop();

	virtual void	Undo() override;

	void	SetCount(const JStyledTextBuffer::TextCount& count);

private:

	JStyledTextBuffer::TextIndex	itsOrigCaretLoc;
	JStyledTextBuffer::TextIndex	itsNewSelStart;
	JStyledTextBuffer::TextIndex	itsNewSelEnd;

private:

	// not allowed

	JTEUndoDrop(const JTEUndoDrop& source);
	const JTEUndoDrop& operator=(const JTEUndoDrop& source);
};

#endif
