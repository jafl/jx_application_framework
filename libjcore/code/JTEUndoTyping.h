/******************************************************************************
 JTEUndoTyping.h

	Interface for the JTEUndoTyping class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTyping
#define _H_JTEUndoTyping

#include <JTEUndoTextBase.h>

class JTEUndoTyping : public JTEUndoTextBase
{
public:

	JTEUndoTyping(JStyledTextBuffer* te, const JStyledTextBuffer::TextIndex& start);

	virtual ~JTEUndoTyping();

	virtual void	Undo() override;

	void	HandleDelete(JStringIterator* iter, const JSize charCount);
	void	HandleFwdDelete(JStringIterator* iter, const JSize charCount);

	void	HandleCharacters(const JStyledTextBuffer::TextCount& count);

private:

	JStyledTextBuffer::TextIndex	itsOrigStartIndex;
	JStyledTextBuffer::TextCount	itsCount;

private:

	// not allowed

	JTEUndoTyping(const JTEUndoTyping& source);
	const JTEUndoTyping& operator=(const JTEUndoTyping& source);
};

#endif
