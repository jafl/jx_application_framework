/******************************************************************************
 JTEUndoTabShift.h

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTabShift
#define _H_JTEUndoTabShift

#include <JTEUndoPaste.h>

class JTEUndoTabShift : public JTEUndoPaste
{
public:

	JTEUndoTabShift(JStyledTextBuffer* te);

	virtual ~JTEUndoTabShift();

private:

	// not allowed

	JTEUndoTabShift(const JTEUndoTabShift& source);
	const JTEUndoTabShift& operator=(const JTEUndoTabShift& source);
};

#endif
