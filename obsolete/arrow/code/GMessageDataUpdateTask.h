/******************************************************************************
 GMessageDataUpdateTask.h

	Interface for the GMessageDataUpdateTask class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMessageDirUpdateTask
#define _H_GMessageDirUpdateTask

#include <JXIdleTask.h>

class GMMailboxData;

class GMessageDataUpdateTask : public JXIdleTask
{
public:

	GMessageDataUpdateTask(GMMailboxData* data);

	virtual ~GMessageDataUpdateTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

	static void		StopUpdate(const JBoolean stop);

private:

	GMMailboxData*		itsData;		// we don't own this
	Time				itsElapsedTime;

	static JBoolean		itsStopUpdate;

private:

	// not allowed

	GMessageDataUpdateTask(const GMessageDataUpdateTask& source);
	const GMessageDataUpdateTask& operator=(const GMessageDataUpdateTask& source);
};

#endif
