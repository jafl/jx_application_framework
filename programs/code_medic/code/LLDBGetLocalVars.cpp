/******************************************************************************
 LLDBGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetLocalVars.h"
#include "CMVarNode.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValueList.h"
#include "lldb/API/SBValue.h"
#include "LLDBLink.h"
#include "LLDBVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetLocalVars::LLDBGetLocalVars
	(
	CMVarNode* rootNode
	)
	:
	CMGetLocalVars(JString::empty),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetLocalVars::~LLDBGetLocalVars()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetLocalVars::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		itsRootNode->DeleteAllChildren();
		return;
		}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (!f.IsValid())
		{
		itsRootNode->DeleteAllChildren();
		return;
		}

	lldb::SBValueList vars = f.GetVariables(true, true, false, false, lldb::eDynamicDontRunTarget);
	if (!vars.IsValid())
		{
		itsRootNode->DeleteAllChildren();
		return;
		}

	// delete existing nodes beyond the first one that doesn't match the
	// new variable names

	const JSize newCount = vars.GetSize();
	JSize origCount      = itsRootNode->GetChildCount();

	while (origCount > newCount)
		{
		jdelete itsRootNode->GetChild(origCount);
		origCount--;
		}

	// origCount <= newCount

	for (JIndex i=1; i<=origCount; i++)
		{
		if (vars.GetValueAtIndex(i-1).GetName() == nullptr)
			{
			continue;
			}

		if ((itsRootNode->GetVarChild(i))->GetName() !=
			vars.GetValueAtIndex(i-1).GetName())
			{
			if (i == 1)		// optimize since likely to be most common case
				{
				itsRootNode->DeleteAllChildren();
				origCount = 0;
				}
			else
				{
				while (origCount >= i)
					{
					jdelete itsRootNode->GetChild(origCount);
					origCount--;
					}
				}
			break;	// for clarity:  would happen anyway
			}
		}

	// update/create nodes

	for (JIndex i=1; i<=newCount; i++)
		{
		lldb::SBValue v = vars.GetValueAtIndex(i-1);
		if (v.IsValid())	// paranoia
			{
			CMVarNode* node = LLDBVarNode::BuildTree(f, v);

			if (i <= origCount)
				{
				itsRootNode->GetVarChild(i)->UpdateValue(node);
				}
			else
				{
				itsRootNode->Append(node);	// avoid automatic update
				}
			}
		}
}
