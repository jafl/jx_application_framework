/******************************************************************************
 CMQuitTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMQuitTask
#define _H_CMQuitTask

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
