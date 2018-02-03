/******************************************************************************
 JTEUndoStyle.h

	Interface for the JTEUndoStyle class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoStyle
#define _H_JTEUndoStyle

#include <JTEUndoBase.h>

class JTEUndoStyle : public JTEUndoBase
{
public:

	JTEUndoStyle(JTextEditor* te);

	virtual ~JTEUndoStyle();

	virtual void	Undo();

	virtual void	SetFont(const JString& name, const JSize size);

private:

	JIndex				itsStartIndex;
	JRunArray<JFont>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoStyle(const JTEUndoStyle& source);
	const JTEUndoStyle& operator=(const JTEUndoStyle& source);
};

#endif
