/******************************************************************************
 JTEUndoStyle.h

	Interface for the JTEUndoStyle class.

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoStyle
#define _H_JTEUndoStyle

#include <JTEUndoBase.h>

class JTEUndoStyle : public JTEUndoBase
{
public:

	JTEUndoStyle(JStyledTextBuffer* buffer, const JStyledTextBuffer::TextRange& range);

	virtual ~JTEUndoStyle();

	virtual void	Undo() override;

	virtual void	SetFont(const JString& name, const JSize size) override;

	JBoolean	SameRange(const JStyledTextBuffer::TextRange& range) const;

private:

	JStyledTextBuffer::TextRange	itsRange;
	JRunArray<JFont>*				itsOrigStyles;

private:

	// not allowed

	JTEUndoStyle(const JTEUndoStyle& source);
	const JTEUndoStyle& operator=(const JTEUndoStyle& source);
};

#endif
