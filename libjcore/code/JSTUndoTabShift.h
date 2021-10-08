/******************************************************************************
 JSTUndoTabShift.h

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTabShift
#define _H_JTEUndoTabShift

#include "jx-af/jcore/JSTUndoPaste.h"

class JSTUndoTabShift : public JSTUndoPaste
{
public:

	JSTUndoTabShift(JStyledText* text, const JStyledText::TextRange& range);

	~JSTUndoTabShift() override;
};

#endif
