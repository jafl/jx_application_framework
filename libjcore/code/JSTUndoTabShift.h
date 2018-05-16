/******************************************************************************
 JSTUndoTabShift.h

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTabShift
#define _H_JTEUndoTabShift

#include <JSTUndoPaste.h>

class JSTUndoTabShift : public JSTUndoPaste
{
public:

	JSTUndoTabShift(JStyledText* text, const JStyledText::TextRange& range);

	virtual ~JSTUndoTabShift();

private:

	// not allowed

	JSTUndoTabShift(const JSTUndoTabShift& source);
	const JSTUndoTabShift& operator=(const JSTUndoTabShift& source);
};

#endif
