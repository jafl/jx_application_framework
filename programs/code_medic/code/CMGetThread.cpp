/******************************************************************************
 CMGetThread.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetThread.h"
#include "CMThreadsWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetThread::CMGetThread
	(
	const JCharacter*	cmd,
	CMThreadsWidget*	widget
	)
	:
	CMCommand(cmd, kJFalse, kJTrue),
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
