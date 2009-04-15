/******************************************************************************
 JTEUndoTabShift.h

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoTabShift
#define _H_JTEUndoTabShift

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoPaste.h>

class JTEUndoTabShift : public JTEUndoPaste
{
public:

	JTEUndoTabShift(JTextEditor* te);

	virtual ~JTEUndoTabShift();

	void	UpdateEndChar();

private:

	// not allowed

	JTEUndoTabShift(const JTEUndoTabShift& source);
	const JTEUndoTabShift& operator=(const JTEUndoTabShift& source);
};

#endif
