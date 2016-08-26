/******************************************************************************
 JTEUndoBase.h

	Copyright © 1996-2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoBase
#define _H_JTEUndoBase

#include <JUndo.h>
#include <JTextEditor.h>	// need defintion of JTextEditor::Font

class JTextEditor;
class JTEUndoTyping;
class JTEUndoStyle;
class JTEUndoTabShift;

class JTEUndoBase : public JUndo
{
public:

	JTEUndoBase(JTextEditor* te);

	virtual ~JTEUndoBase();

	// called by JTextEditor::SetAllFontNameAndSize()

	virtual void	SetFont(const JCharacter* name, const JSize size);

	// required by some derived classes

	virtual void	SetPasteLength(const JSize length);

protected:

	JTextEditor*	GetTE() const;

	void	SetFont(JRunArray<JFont>* styles,
					const JCharacter* name, const JSize size);

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
