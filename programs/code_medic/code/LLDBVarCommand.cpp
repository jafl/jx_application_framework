/******************************************************************************
 LLDBVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBVarCommand.h"
#include "CMVarNode.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"
#include "LLDBLink.h"
#include "LLDBVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBVarCommand::LLDBVarCommand
	(
	const JString& expr
	)
	:
	CMVarCommand()
{
	itsExpr = expr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBVarCommand::~LLDBVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBVarCommand::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		return;
		}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (!f.IsValid())
		{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		return;
		}

	lldb::SBValue v = f.EvaluateExpression(itsExpr.GetBytes(), lldb::eDynamicDontRunTarget);
	if (v.IsValid())
		{
		Broadcast(ValueMessage(kValueUpdated, LLDBVarNode::BuildTree(f, v)));
		}
	else
		{
		Broadcast(ValueMessage(kValueFailed, nullptr));
		}
}
