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

	virtual void	Undo() override;

	void	HandleDelete(JStringIterator* iter, const JSize charCount);
	void	HandleFwdDelete(JStringIterator* iter, const JSize charCount);

	void	HandleCharacters(const JTextEditor::TextCount& count);

private:

	JTextEditor::TextIndex	itsOrigStartIndex;
	JTextEditor::TextCount	itsCount;

private:

	// not allowed

	JTEUndoTyping(const JTEUndoTyping& source);
	const JTEUndoTyping& operator=(const JTEUndoTyping& source);
};

#endif
