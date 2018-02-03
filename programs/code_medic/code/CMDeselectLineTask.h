/******************************************************************************
 CMDeselectLineTask.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMDeselectLineTask
#define _H_CMDeselectLineTask

#include <JXIdleTask.h>

class CMLineIndexTable;

class CMDeselectLineTask : public JXIdleTask
{
public:

	CMDeselectLineTask(CMLineIndexTable* table);

	virtual ~CMDeselectLineTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	CMLineIndexTable*	itsTable;

private:

	// not allowed

	CMDeselectLineTask(const CMDeselectLineTask& source);
	const CMDeselectLineTask& operator=(const CMDeselectLineTask& source);
};

#endif
