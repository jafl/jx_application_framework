/******************************************************************************
 JXCreatePG.h

	Interface for JXCreatePG class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCreatePG
#define _H_JXCreatePG

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JCreateProgressDisplay.h>

class JXCreatePG : public JCreateProgressDisplay
{
public:

	JXCreatePG();

	virtual ~JXCreatePG();

	virtual JProgressDisplay* New();

private:

	// not allowed

	JXCreatePG(const JXCreatePG& source);
	const JXCreatePG& operator=(const JXCreatePG& source);
};

#endif
