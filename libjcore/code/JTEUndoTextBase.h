/******************************************************************************
 JTEUndoTextBase.h

	Interface for the JTEUndoTextBase class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTextBase
#define _H_JTEUndoTextBase

#include <JTEUndoBase.h>

class JTEUndoTextBase : public JTEUndoBase
{
public:

	JTEUndoTextBase(JStyledText* text, const JStyledText::TextRange& range);

	virtual ~JTEUndoTextBase();

	virtual void	SetFont(const JString& name, const JSize size) override;

protected:

	void	UndoText(const JStyledText::TextRange& range);

	void	PrependToSave(const JUtf8Character& c, const JIndex charIndex);
	void	AppendToSave(const JUtf8Character& c, const JIndex charIndex);

private:

	JString				itsOrigText;
	JRunArray<JFont>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoTextBase(const JTEUndoTextBase& source);
	const JTEUndoTextBase& operator=(const JTEUndoTextBase& source);
};

#endif
