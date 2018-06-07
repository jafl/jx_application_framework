/******************************************************************************
 LLDBGetStack.cpp

	BASE CLASS = CMGetStack

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetStack.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBLineEntry.h"
#include "lldb/API/SBFileSpec.h"
#include "lldb/API/SBValueList.h"
#include "lldb/API/SBValue.h"
#include "CMStackFrameNode.h"
#include "CMStackArgNode.h"
#include "CMStackWidget.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetStack::LLDBGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
	:
	CMGetStack("", tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetStack::~LLDBGetStack()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex assertPattern = "^(JAssert|__assert(_[^[:space:]]+)?)\\(";

void
LLDBGetStack::HandleSuccess
	(
	const JString& cmdData
	)
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		return;
		}

	lldb::SBThread t = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (!t.IsValid())
		{
		return;
		}

	JTreeNode* root       = GetTree()->GetRoot();
	JIndex initFrameIndex = 0;

	const JSize frameCount = t.GetNumFrames();
	JString frameName, fileName;
	JBoolean selectNextFrame = kJFalse;
	for (JIndex i=0; i<frameCount; i++)
		{
		lldb::SBFrame f = t.GetFrameAtIndex(i);

		frameName = JString(i, JString::kBase10);
		while (frameName.GetLength() < kFrameIndexWidth)
			{
			frameName.PrependCharacter('0');
			}
		frameName += ":  ";

		const JCharacter* name = f.GetFunctionName();
		frameName += (name == nullptr ? "?" : name);

		JIndex lineIndex      = 0;
		lldb::SBLineEntry e   = f.GetLineEntry();
		lldb::SBFileSpec file = e.GetFileSpec();
		if (file.IsValid())
			{
			fileName  = JCombinePathAndName(file.GetDirectory(), file.GetFilename());
			lineIndex = e.GetLine();
			}
		else
			{
			fileName.Clear();
			}

		CMStackFrameNode* node =
			jnew CMStackFrameNode(root, f.GetFrameID(), frameName,
								 fileName, lineIndex);
		assert( node != nullptr );
		root->Prepend(node);

		if (selectNextFrame)
			{
			initFrameIndex  = f.GetFrameID();
			selectNextFrame = kJFalse;
			}
		else if (f.GetFunctionName() != nullptr && assertPattern.Match(f.GetFunctionName()))
			{
			selectNextFrame = kJTrue;
			}

		// arguments

		lldb::SBValueList args = f.GetVariables(true, false, false, true, lldb::eDynamicDontRunTarget);
		if (!args.IsValid())
			{
			continue;
			}

		for (JIndex i=0; i<args.GetSize(); i++)
			{
			lldb::SBValue v = args.GetValueAtIndex(i);

			if (v.GetName() != nullptr)
				{
				CMStackArgNode* argNode = jnew CMStackArgNode(node, v.GetName(), v.GetValue() == nullptr ? "null" : v.GetValue());
				assert( argNode != nullptr );
				}
			}
		}

	GetWidget()->FinishedLoading(initFrameIndex);
}
