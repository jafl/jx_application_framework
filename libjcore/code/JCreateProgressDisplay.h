/******************************************************************************
 JCreateProgressDisplay.h

	Interface for JCreateProgressDisplay class.

	Copyright (C) 1995-96 by John Lindal.

 ******************************************************************************/

#ifndef _H_JCreateProgressDisplay
#define _H_JCreateProgressDisplay

#include "JProgressDisplay.h"	// for convenience

class JCreateProgressDisplay
{
public:

	JCreateProgressDisplay();

	virtual ~JCreateProgressDisplay();

	virtual JProgressDisplay* New() = 0;

private:

	// not allowed

	JCreateProgressDisplay(const JCreateProgressDisplay&) = delete;
	JCreateProgressDisplay& operator=(const JCreateProgressDisplay&) = delete;
};

#endif
