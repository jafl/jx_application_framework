/******************************************************************************
 JTEUndoTyping.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTyping
#define _H_JTEUndoTyping

#include <JTEUndoTextBase.h>

class JStringMatch;

class JTEUndoTyping : public JTEUndoTextBase
{
public:

	JTEUndoTyping(JStyledTextBuffer* buffer, const JStyledTextBuffer::TextIndex& start);

	virtual ~JTEUndoTyping();

	virtual void	Undo() override;

	void	HandleDelete(const JStringMatch& match);
	void	HandleFwdDelete(const JStringMatch& match);

	void	HandleCharacters(const JStyledTextBuffer::TextCount& count);

	JBoolean	SameStartIndex(const JStyledTextBuffer::TextIndex& index) const;

private:

	JStyledTextBuffer::TextIndex	itsOrigStartIndex;
	JStyledTextBuffer::TextCount	itsCount;

private:

	// not allowed

	JTEUndoTyping(const JTEUndoTyping& source);
	const JTEUndoTyping& operator=(const JTEUndoTyping& source);
};

#endif
