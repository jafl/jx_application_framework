/******************************************************************************
 CMQuitTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMQuitTask
#define _H_CMQuitTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class CMQuitTask : public JXIdleTask
{
public:

	CMQuitTask();

	virtual ~CMQuitTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	// not allowed

	CMQuitTask(const CMQuitTask& source);
	const CMQuitTask& operator=(const CMQuitTask& source);
};

#endif
