/******************************************************************************
 JTEUndoPaste.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste
#define _H_JTEUndoPaste

#include <JTEUndoTextBase.h>

class JTEUndoPaste : public JTEUndoTextBase
{
public:

	JTEUndoPaste(JStyledText* text, const JStyledText::TextRange& range);

	virtual ~JTEUndoPaste();

	virtual void	Undo() override;

	virtual void	SetCount(const JStyledText::TextCount& count) override;

	JBoolean	SameStartIndex(const JStyledText::TextRange& range) const;

private:

	JStyledText::TextRange itsRange;

private:

	// not allowed

	JTEUndoPaste(const JTEUndoPaste& source);
	const JTEUndoPaste& operator=(const JTEUndoPaste& source);
};

#endif
