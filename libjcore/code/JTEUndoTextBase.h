/******************************************************************************
 JTEUndoTextBase.h

	Interface for the JTEUndoTextBase class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoTextBase
#define _H_JTEUndoTextBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoBase.h>

class JTEUndoTextBase : public JTEUndoBase
{
public:

	JTEUndoTextBase(JTextEditor* te);

	virtual ~JTEUndoTextBase();

	virtual void	Undo();

	virtual void	SetFont(const JCharacter* name, const JSize size);

protected:

	void	PrepareForUndo(const JIndex start, const JSize length);

	void	PrependToSave(const JIndex index);
	void	AppendToSave(const JIndex index);

private:

	JString*						itsOrigBuffer;
	JRunArray<JTextEditor::Font>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoTextBase(const JTEUndoTextBase& source);
	const JTEUndoTextBase& operator=(const JTEUndoTextBase& source);
};

#endif
