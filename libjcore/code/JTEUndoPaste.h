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

	JTEUndoPaste(JTextEditor* te, const JTextEditor::TextCount& pasteCount);

	virtual ~JTEUndoPaste();

	virtual void	SetPasteCount(const JTextEditor::TextCount& count) override;
	virtual void	Undo() override;

protected:

	void	SetCurrentCount(const JTextEditor::TextCount& count);

private:

	JTextEditor::TextIndex	itsOrigSelStart;
	JTextEditor::TextCount	itsCount;

private:

	// not allowed

	JTEUndoPaste(const JTEUndoPaste& source);
	const JTEUndoPaste& operator=(const JTEUndoPaste& source);
};


/******************************************************************************
 SetCurrentCount (protected)

 ******************************************************************************/

inline void
JTEUndoPaste::SetCurrentCount
	(
	const JTextEditor::TextCount& count
	)
{
	itsCount = count;
}

#endif
