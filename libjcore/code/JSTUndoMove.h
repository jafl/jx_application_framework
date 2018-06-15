/******************************************************************************
 JSTUndoMove.h

	Interface for the JSTUndoMove class.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoMove
#define _H_JTEUndoMove

#include "JSTUndoBase.h"

class JSTUndoMove : public JSTUndoBase
{
public:

	JSTUndoMove(JStyledText* text,
				const JStyledText::TextIndex& srcIndex,
				const JStyledText::TextIndex& destIndex,
				const JStyledText::TextCount& count);

	virtual ~JSTUndoMove();

	virtual void	Undo() override;

	virtual void	SetCount(const JStyledText::TextCount& count) override;

private:

	JStyledText::TextIndex	itsSrcIndex;
	JStyledText::TextIndex	itsDestIndex;
	JStyledText::TextCount	itsCount;

private:

	// not allowed

	JSTUndoMove(const JSTUndoMove& source);
	const JSTUndoMove& operator=(const JSTUndoMove& source);
};

#endif
