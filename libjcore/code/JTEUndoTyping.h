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

	JTEUndoTyping(JTextEditor* te);

	virtual ~JTEUndoTyping();

	virtual void	Undo();

	void	HandleCharacter();
	void	HandleDelete(const JIndex firstChar, const JIndex lastChar);
	void	HandleFwdDelete(const JIndex firstChar, const JIndex lastChar);

	void	HandleCharacters(const JSize count);
	void	HandleAutoIndentDelete(const JIndex firstChar, const JIndex lastChar);

private:

	JIndex	itsOrigStartIndex;
	JSize	itsLength;

private:

	// not allowed

	JTEUndoTyping(const JTEUndoTyping& source);
	const JTEUndoTyping& operator=(const JTEUndoTyping& source);
};


/******************************************************************************
 HandleCharacter

 ******************************************************************************/

inline void
JTEUndoTyping::HandleCharacter()
{
	HandleCharacters(1);
}

#endif
