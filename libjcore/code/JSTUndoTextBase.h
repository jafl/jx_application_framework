/******************************************************************************
 JSTUndoTextBase.h

	Interface for the JSTUndoTextBase class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTextBase
#define _H_JTEUndoTextBase

#include "JSTUndoBase.h"

class JSTUndoTextBase : public JSTUndoBase
{
public:

	JSTUndoTextBase(JStyledText* text, const JStyledText::TextRange& range);

	~JSTUndoTextBase() override;

	void	SetFont(const JString& name, const JSize size) override;

protected:

	void	UndoText(const JStyledText::TextRange range);

	void	PrependToSave(const JUtf8Character& c, const JIndex charIndex);
	void	AppendToSave(const JUtf8Character& c, const JIndex charIndex);

private:

	JString				itsOrigText;
	JRunArray<JFont>*	itsOrigStyles;
};

#endif
