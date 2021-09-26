/******************************************************************************
 JXCloseDirectorTask.cpp

	This is an idle task rather than an urgent task because it might be
	unsafe to perform until after the idle task stack has returned to the
	same level, e.g., after blocking dialogs have finished.

	BASE CLASS = JXIdleTask

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXCloseDirectorTask.h"
#include "jx-af/jx/JXDirector.h"
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
	assert( task != nullptr );
	task->Start();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXCloseDirectorTask::JXCloseDirectorTask
	(
	JXDirector* director
	)
	:
	JXIdleTask(0),
	itsDirector(director)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCloseDirectorTask::~JXCloseDirectorTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXCloseDirectorTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	itsDirector->Close();
	itsDirector = nullptr;
	jdelete this;
}
