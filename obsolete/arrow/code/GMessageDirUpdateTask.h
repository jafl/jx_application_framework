/******************************************************************************
 GMessageDirUpdateTask.h

	Interface for the GMessageDirUpdateTask class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMessageDirUpdateTask
#define _H_GMessageDirUpdateTask

#include <JXIdleTask.h>

class GMessageTableDir;

class GMessageDirUpdateTask : public JXIdleTask
{
public:

	GMessageDirUpdateTask(GMessageTableDir* dir);

	virtual ~GMessageDirUpdateTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

	static void		StopUpdate(const JBoolean stop);

private:

	GMessageTableDir*	itsDir;		// we don't own this
	Time				itsElapsedTime;

	static JBoolean		itsStopUpdate;

private:

	// not allowed

	GMessageDirUpdateTask(const GMessageDirUpdateTask& source);
	const GMessageDirUpdateTask& operator=(const GMessageDirUpdateTask& source);
};

#endif
