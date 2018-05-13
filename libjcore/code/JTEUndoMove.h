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

	JTEUndoMove(JStyledText* text,
				const JStyledText::TextIndex& srcIndex,
				const JStyledText::TextIndex& destIndex,
				const JStyledText::TextCount& count);

	virtual ~JTEUndoMove();

	virtual void	Undo() override;

	virtual void	SetCount(const JStyledText::TextCount& count) override;

private:

	JStyledText::TextIndex	itsSrcIndex;
	JStyledText::TextIndex	itsDestIndex;
	JStyledText::TextCount	itsCount;

private:

	// not allowed

	JTEUndoMove(const JTEUndoMove& source);
	const JTEUndoMove& operator=(const JTEUndoMove& source);
};

#endif
