/******************************************************************************
 CMDeselectLineTask.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDeselectLineTask
#define _H_CMDeselectLineTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
