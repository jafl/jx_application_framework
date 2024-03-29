/******************************************************************************
 JXCloseDirectorTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXCloseDirectorTask.h"
#include "JXDirector.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Close (static)

 ******************************************************************************/

void
JXCloseDirectorTask::Close
	(
	JXDirector* director
	)
{
	auto* task = jnew JXCloseDirectorTask(director);
	task->Go();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXCloseDirectorTask::JXCloseDirectorTask
	(
	JXDirector* director
	)
	:
	JXUrgentTask(director),
	itsDirector(director)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXCloseDirectorTask::~JXCloseDirectorTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXCloseDirectorTask::Perform()
{
	itsDirector->Close();
}
