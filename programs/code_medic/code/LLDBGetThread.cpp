/******************************************************************************
 LLDBGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetThread.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "CMThreadsWidget.h"
#include "LLDBGetThreads.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetThread::LLDBGetThread
	(
	CMThreadsWidget* widget
	)
	:
	CMGetThread("", widget)
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
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		return;
		}

	lldb::SBProcess p = link->GetDebugger()->GetSelectedTarget().GetProcess();
	if (!p.IsValid())
		{
		return;
		}

	(GetWidget())->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
