/******************************************************************************
 GLClosePlotDirTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <GLClosePlotDirTask.h>
#include "PlotDir.h"

#include <JXApplication.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLClosePlotDirTask::GLClosePlotDirTask
	(
	PlotDir* dir
	)
	:
	JXUrgentTask()
{
	itsDir = dir;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLClosePlotDirTask::~GLClosePlotDirTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GLClosePlotDirTask::Perform()
{
	itsDir->Close();
}
