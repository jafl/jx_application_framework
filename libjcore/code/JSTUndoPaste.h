/******************************************************************************
 JSTUndoPaste.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste
#define _H_JTEUndoPaste

#include "jx-af/jcore/JSTUndoTextBase.h"

class JSTUndoPaste : public JSTUndoTextBase
{
public:

	JSTUndoPaste(JStyledText* text, const JStyledText::TextRange& range);

	~JSTUndoPaste() override;

	void	Undo() override;

	void	SetCount(const JStyledText::TextCount& count) override;

	bool	SameStartIndex(const JStyledText::TextRange& range) const;

private:

	JStyledText::TextRange itsRange;
};

#endif
