/******************************************************************************
 JTEUndoMove.h

	Interface for the JTEUndoMove class.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoMove
#define _H_JTEUndoMove

#include <JTEUndoBase.h>

class JTEUndoMove : public JTEUndoBase
{
public:

	JTEUndoMove(JStyledTextBuffer* buffer,
				const JStyledTextBuffer::TextIndex& srcIndex,
				const JStyledTextBuffer::TextIndex& destIndex,
				const JStyledTextBuffer::TextCount& count);

	virtual ~JTEUndoMove();

	virtual void	Undo() override;

	virtual void	SetCount(const JStyledTextBuffer::TextCount& count) override;

private:

	JStyledTextBuffer::TextIndex	itsSrcIndex;
	JStyledTextBuffer::TextIndex	itsDestIndex;
	JStyledTextBuffer::TextCount	itsCount;

private:

	// not allowed

	JTEUndoMove(const JTEUndoMove& source);
	const JTEUndoMove& operator=(const JTEUndoMove& source);
};

#endif
