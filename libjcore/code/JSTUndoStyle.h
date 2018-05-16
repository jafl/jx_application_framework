/******************************************************************************
 JSTUndoStyle.h

	Interface for the JSTUndoStyle class.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoStyle
#define _H_JTEUndoStyle

#include <JSTUndoBase.h>

class JSTUndoStyle : public JSTUndoBase
{
public:

	JSTUndoStyle(JStyledText* text, const JStyledText::TextRange& range);

	virtual ~JSTUndoStyle();

	virtual void	Undo() override;

	virtual void	SetFont(const JString& name, const JSize size) override;

	JBoolean	SameRange(const JStyledText::TextRange& range) const;

private:

	JStyledText::TextRange	itsRange;
	JRunArray<JFont>*		itsOrigStyles;

private:

	// not allowed

	JSTUndoStyle(const JSTUndoStyle& source);
	const JSTUndoStyle& operator=(const JSTUndoStyle& source);
};

#endif
