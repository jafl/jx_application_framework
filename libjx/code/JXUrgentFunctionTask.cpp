/******************************************************************************
 JXUrgentFunctionTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2023 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXUrgentFunctionTask.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUrgentFunctionTask::JXUrgentFunctionTask
	(
	JBroadcaster* 					target,
	const std::function<void()>&	f,
	const JString&					name
	)
	:
	JXUrgentTask(target),
	itsFunction(f),
	itsName(jnew JString(name))
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXUrgentFunctionTask::~JXUrgentFunctionTask()
{
	jdelete itsName;
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXUrgentFunctionTask::Perform()
{
	itsFunction();
}
