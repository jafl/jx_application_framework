/******************************************************************************
 JCreateTextPG.h

	Interface for JCreateTextPG class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JCreateTextPG
#define _H_JCreateTextPG

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JCreateProgressDisplay.h>

class JCreateTextPG : public JCreateProgressDisplay
{
public:

	JCreateTextPG();

	virtual ~JCreateTextPG();

	virtual JProgressDisplay* New();

private:

	// not allowed

	JCreateTextPG(const JCreateTextPG& source);
	const JCreateTextPG& operator=(const JCreateTextPG& source);
};

#endif
