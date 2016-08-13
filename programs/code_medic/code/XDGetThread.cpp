/******************************************************************************
 XDGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XDGetThread.h"
#include "CMThreadsWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetThread::XDGetThread
	(
	CMThreadsWidget* widget
	)
	:
	CMGetThread("status", widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetThread::~XDGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetThread::HandleSuccess
	(
	const JString& data
	)
{
	GetWidget()->FinishedLoading(0);
}
