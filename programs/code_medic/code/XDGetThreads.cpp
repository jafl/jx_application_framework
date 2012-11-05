/******************************************************************************
 XDGetThreads.cpp

	BASE CLASS = CMGetThreads

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDGetThreads.h"
#include "CMThreadsWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetThreads::XDGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
	:
	CMGetThreads("status", widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetThreads::~XDGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetThreads::HandleSuccess
	(
	const JString& data
	)
{
	(GetWidget())->FinishedLoading(0);
}
