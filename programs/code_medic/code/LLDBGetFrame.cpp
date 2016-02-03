/******************************************************************************
 LLDBGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetFrame.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "CMStackWidget.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
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
	CMGetFrame(""),
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
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		return;
		}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
		{
		itsWidget->SelectFrame(f.GetFrameID());
		}
}
