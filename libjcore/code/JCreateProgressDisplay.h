/******************************************************************************
 JCreateProgressDisplay.h

	Interface for JCreateProgressDisplay class.

	Copyright © 1995-96 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JCreateProgressDisplay
#define _H_JCreateProgressDisplay

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JProgressDisplay.h>	// for convenience

class JCreateProgressDisplay
{
public:

	JCreateProgressDisplay();

	virtual ~JCreateProgressDisplay();

	virtual JProgressDisplay* New() = 0;

private:

	// not allowed

	JCreateProgressDisplay(const JCreateProgressDisplay& source);
	const JCreateProgressDisplay& operator=(const JCreateProgressDisplay& source);
};

#endif
