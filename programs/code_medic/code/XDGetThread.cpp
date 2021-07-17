/******************************************************************************
 XDGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright (C) 2007 by John Lindal.

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
	CMGetThread(JString("status", JString::kNoCopy), widget)
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
