/******************************************************************************
 JSTUndoPaste.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste
#define _H_JTEUndoPaste

#include "JSTUndoTextBase.h"

class JSTUndoPaste : public JSTUndoTextBase
{
public:

	JSTUndoPaste(JStyledText* text, const JStyledText::TextRange& range);

	virtual ~JSTUndoPaste();

	virtual void	Undo() override;

	virtual void	SetCount(const JStyledText::TextCount& count) override;

	bool	SameStartIndex(const JStyledText::TextRange& range) const;

private:

	JStyledText::TextRange itsRange;

private:

	// not allowed

	JSTUndoPaste(const JSTUndoPaste& source);
	const JSTUndoPaste& operator=(const JSTUndoPaste& source);
};

#endif
