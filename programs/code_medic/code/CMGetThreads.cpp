/******************************************************************************
 CMGetThreads.cpp

	BASE CLASS = CMCommand

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetThreads.h"
#include "CMThreadsWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetThreads::CMGetThreads
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
