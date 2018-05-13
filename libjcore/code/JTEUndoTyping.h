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

	JTEUndoTyping(JStyledText* buffer, const JStyledText::TextIndex& start);
	JTEUndoTyping(JStyledText* buffer, const JStyledText::TextRange& replaceRange);

	virtual ~JTEUndoTyping();

	virtual void	Undo() override;

	void	HandleDelete(const JStringMatch& match);
	void	HandleFwdDelete(const JStringMatch& match);

	void	HandleCharacters(const JStyledText::TextCount& count);

	JBoolean	SameStartIndex(const JStyledText::TextIndex& index) const;

private:

	JStyledText::TextIndex	itsOrigStartIndex;
	JStyledText::TextCount	itsCount;

private:

	// not allowed

	JTEUndoTyping(const JTEUndoTyping& source);
	const JTEUndoTyping& operator=(const JTEUndoTyping& source);
};

#endif
