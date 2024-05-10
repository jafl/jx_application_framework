/******************************************************************************
 JXFunctionTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "JXFunctionTask.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFunctionTask::JXFunctionTask
	(
	const Time						period,
	const std::function<void()>&	f,
	const JString&					name,
	const bool						oneShot
	)
	:
	JXIdleTask(period),
	itsFunction(f),
	itsName(jnew JString(name)),
	itsIsOneShotFlag(oneShot)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFunctionTask::~JXFunctionTask()
{
	jdelete itsName;
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
