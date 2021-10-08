/******************************************************************************
 JXQuitIfAllDeactTask.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXQuitIfAllDeactTask
#define _H_JXQuitIfAllDeactTask

#include "jx-af/jx/JXIdleTask.h"

class JXQuitIfAllDeactTask : public JXIdleTask
{
public:

	JXQuitIfAllDeactTask();

	~JXQuitIfAllDeactTask();

	void	Perform(const Time delta, Time* maxSleepTime) override;
};

#endif
