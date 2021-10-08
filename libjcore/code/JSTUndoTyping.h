/******************************************************************************
 JSTUndoTyping.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTyping
#define _H_JTEUndoTyping

#include "jx-af/jcore/JSTUndoTextBase.h"

class JStringMatch;

class JSTUndoTyping : public JSTUndoTextBase
{
public:

	JSTUndoTyping(JStyledText* buffer, const JStyledText::TextIndex& start);
	JSTUndoTyping(JStyledText* buffer, const JStyledText::TextRange& replaceRange);

	~JSTUndoTyping() override;

	void	Undo() override;

	void	HandleDelete(const JStringMatch& match);
	void	HandleFwdDelete(const JStringMatch& match);

	void	HandleCharacters(const JStyledText::TextCount& count);

	bool	MatchesCurrentIndex(const JStyledText::TextIndex& index) const;

private:

	JStyledText::TextIndex	itsOrigStartIndex;
	JStyledText::TextCount	itsCount;
};

#endif
