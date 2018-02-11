/******************************************************************************
 JTEUndoBase.h

	Copyright (C) 1996-2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoBase
#define _H_JTEUndoBase

#include <JUndo.h>
#include <JTextEditor.h>	// need defintion of JTextEditor::Font

class JTEUndoBase : public JUndo
{
public:

	JTEUndoBase(JTextEditor* te);

	virtual ~JTEUndoBase();

	// called by JTextEditor::SetAllFontNameAndSize()

	virtual void	SetFont(const JString& name, const JSize size);

	// required by some derived classes

	virtual void	SetPasteCount(const JTextEditor::TextCount& count);

protected:

	JTextEditor*	GetTE() const;

	void	SetFont(JRunArray<JFont>* styles,
					const JString& name, const JSize size);

private:

	JTextEditor*	itsTE;		// we don't own this

private:

	// not allowed

	JTEUndoBase(const JTEUndoBase& source);
	const JTEUndoBase& operator=(const JTEUndoBase& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JTextEditor*
JTEUndoBase::GetTE()
	const
{
	return itsTE;
}

#endif
