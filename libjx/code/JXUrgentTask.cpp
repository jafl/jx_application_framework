/******************************************************************************
 JXUrgentTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXUrgentTask.h"
#include "jXGlobals.h"

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
	JXGetApplication()->RemoveUrgentTask(this);
}

/******************************************************************************
 Go

 ******************************************************************************/

void
JXUrgentTask::Go()
{
	JXGetApplication()->InstallUrgentTask(this);
}
