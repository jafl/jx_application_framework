/******************************************************************************
 LLDBGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetThread.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "CMThreadsWidget.h"
#include "LLDBGetThreads.h"
#include "cmGlobals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetThread::LLDBGetThread
	(
	CMThreadsWidget* widget
	)
	:
	CMGetThread(JString::empty, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetThread::~LLDBGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetThread::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		return;
		}

	lldb::SBProcess p = link->GetDebugger()->GetSelectedTarget().GetProcess();
	if (!p.IsValid())
		{
		return;
		}

	GetWidget()->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
