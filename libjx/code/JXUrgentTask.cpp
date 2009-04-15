/******************************************************************************
 JXUrgentTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXUrgentTask.h>
#include <jXGlobals.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUrgentTask::JXUrgentTask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUrgentTask::~JXUrgentTask()
{
	(JXGetApplication())->RemoveUrgentTask(this);
}

#define JTemplateType JXUrgentTask
#include <JPtrArray.tmpls>
#undef JTemplateType
