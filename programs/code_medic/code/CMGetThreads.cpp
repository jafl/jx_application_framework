/******************************************************************************
 CMGetThreads.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMGetThreads.h"
#include "CMThreadsWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetThreads::CMGetThreads
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

CMGetThreads::~CMGetThreads()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetThreads::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
