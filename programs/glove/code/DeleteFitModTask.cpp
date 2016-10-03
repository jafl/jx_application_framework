/******************************************************************************
 DeleteFitModTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <DeleteFitModTask.h>
#include "FitModule.h"

#include <JXApplication.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DeleteFitModTask::DeleteFitModTask
	(
	FitModule* module
	)
	:
	JXUrgentTask()
{
	itsModule = module;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DeleteFitModTask::~DeleteFitModTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
DeleteFitModTask::Perform()
{
	delete itsModule;
}
