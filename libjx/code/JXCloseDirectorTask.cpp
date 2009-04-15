/******************************************************************************
 JXCloseDirectorTask.cpp

	This is an idle task rather than an urgent task because it might be
	unsafe to perform until after the idle task stack has returned to the
	same level, e.g., after blocking dialogs have finished.

	BASE CLASS = JXIdleTask

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCloseDirectorTask.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Close (static)

 ******************************************************************************/

void
JXCloseDirectorTask::Close
	(
	JXDirector* director
	)
{
	JXCloseDirectorTask* task = new JXCloseDirectorTask(director);
	assert( task != NULL );
	(JXGetApplication())->InstallIdleTask(task);
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
	itsDirector = NULL;
	delete this;
}
