/******************************************************************************
 JTEUndoPaste.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste
#define _H_JTEUndoPaste

#include <JTEUndoTextBase.h>

class JTEUndoPaste : public JTEUndoTextBase
{
public:

	JTEUndoPaste(JTextEditor* te, const JTextEditor::TextCount& pasteLength);

	virtual ~JTEUndoPaste();

	virtual void	SetPasteLength(const JTextEditor::TextCount& length);
	virtual void	Undo();

protected:

	void	SetCurrentLength(const JTextEditor::TextCount& length);

private:

	JTextEditor::TextIndex	itsOrigSelStart;
	JTextEditor::TextCount	itsLength;

private:

	// not allowed

	JTEUndoPaste(const JTEUndoPaste& source);
	const JTEUndoPaste& operator=(const JTEUndoPaste& source);
};


/******************************************************************************
 SetCurrentLength (protected)

 ******************************************************************************/

inline void
JTEUndoPaste::SetCurrentLength
	(
	const JTextEditor::TextCount& length
	)
{
	itsLength = length;
}

#endif
