/******************************************************************************
 CMGetThread.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMGetThread.h"
#include "CMThreadsWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetThread::CMGetThread
	(
	const JString&		cmd,
	CMThreadsWidget*	widget
	)
	:
	CMCommand(cmd, false, true),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetThread::~CMGetThread()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetThread::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
