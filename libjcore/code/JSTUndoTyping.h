/******************************************************************************
 JSTUndoTyping.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTyping
#define _H_JTEUndoTyping

#include <JSTUndoTextBase.h>

class JStringMatch;

class JSTUndoTyping : public JSTUndoTextBase
{
public:

	JSTUndoTyping(JStyledText* buffer, const JStyledText::TextIndex& start);
	JSTUndoTyping(JStyledText* buffer, const JStyledText::TextRange& replaceRange);

	virtual ~JSTUndoTyping();

	virtual void	Undo() override;

	void	HandleDelete(const JStringMatch& match);
	void	HandleFwdDelete(const JStringMatch& match);

	void	HandleCharacters(const JStyledText::TextCount& count);

	JBoolean	MatchesCurrentIndex(const JStyledText::TextIndex& index) const;

private:

	JStyledText::TextIndex	itsOrigStartIndex;
	JStyledText::TextCount	itsCount;

private:

	// not allowed

	JSTUndoTyping(const JSTUndoTyping& source);
	const JSTUndoTyping& operator=(const JSTUndoTyping& source);
};

#endif
