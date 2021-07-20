/******************************************************************************
 LLDBGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetFrame.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "CMStackWidget.h"
#include "cmGlobals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetFrame::LLDBGetFrame
	(
	CMStackWidget* widget
	)
	:
	CMGetFrame(JString::empty),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetFrame::~LLDBGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetFrame::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		return;
		}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
		{
		itsWidget->SelectFrame(f.GetFrameID());
		}
}
