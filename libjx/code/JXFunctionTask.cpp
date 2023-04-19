/******************************************************************************
 JXFunctionTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "JXFunctionTask.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFunctionTask::JXFunctionTask
	(
	const Time						period,
	const std::function<void()>&	f,
	const bool						oneShot
	)
	:
	JXIdleTask(period),
	itsFunction(f),
	itsIsOneShotFlag(oneShot)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFunctionTask::~JXFunctionTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXFunctionTask::Perform
	(
	const Time delta
	)
{
	itsFunction();
	if (itsIsOneShotFlag)
	{
		jdelete this;
	}
}
