/******************************************************************************
 CMDeselectLineNumberTask.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDeselectLineNumberTask
#define _H_CMDeselectLineNumberTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class CMLineNumberTable;

class CMDeselectLineNumberTask : public JXIdleTask
{
public:

	CMDeselectLineNumberTask(CMLineNumberTable* table);

	virtual ~CMDeselectLineNumberTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	CMLineNumberTable*	itsTable;

private:

	// not allowed

	CMDeselectLineNumberTask(const CMDeselectLineNumberTask& source);
	const CMDeselectLineNumberTask& operator=(const CMDeselectLineNumberTask& source);
};

#endif
