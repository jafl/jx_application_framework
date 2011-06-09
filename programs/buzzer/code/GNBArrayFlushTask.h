/******************************************************************************
 GNBArrayFlushTask.h

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBArrayFlushTask
#define _H_GNBArrayFlushTask

#include <JXIdleTask.h>

class GNBApp;

class GNBArrayFlushTask : public JXIdleTask
{
public:

	GNBArrayFlushTask(GNBApp* app);

	virtual ~GNBArrayFlushTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	GNBApp* itsApp;

private:

	// not allowed

	GNBArrayFlushTask(const GNBArrayFlushTask& source);
	const GNBArrayFlushTask& operator=(const GNBArrayFlushTask& source);

};

#endif
