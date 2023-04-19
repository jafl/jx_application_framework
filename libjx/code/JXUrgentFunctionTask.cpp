/******************************************************************************
 JXUrgentFunctionTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2023 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXUrgentFunctionTask.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUrgentFunctionTask::JXUrgentFunctionTask
	(
	JBroadcaster* 					target,
	const std::function<void()>&	f
	)
	:
	JXUrgentTask(target),
	itsFunction(f)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXUrgentFunctionTask::~JXUrgentFunctionTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXUrgentFunctionTask::Perform()
{
	itsFunction();
}
