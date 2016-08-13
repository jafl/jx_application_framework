/******************************************************************************
 LLDBGetThreads.cpp

	BASE CLASS = CMGetThreads

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "LLDBGetThreads.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBStream.h"
#include "CMThreadsWidget.h"
#include "CMThreadNode.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jAssert.h>

const JSize kThreadIndexWidth = 2;	// width of thread index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetThreads::LLDBGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
	:
	CMGetThreads("", widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetThreads::~LLDBGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex threadIDPattern = "^thread #([[:digit:]]+):\\s*tid = [^,]+,";

void
LLDBGetThreads::HandleSuccess
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

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = p.GetNumThreads();
	JString fileName, name, indexStr;
	JArray<JIndexRange> matchList;
	for (JIndex i=0; i<count; i++)
		{
		lldb::SBThread t = p.GetThreadAtIndex(i);
		lldb::SBFrame f  = t.GetSelectedFrame();

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

		lldb::SBStream stream;
		t.GetDescription(stream);
		name = stream.GetData();
		
		if (threadIDPattern.Match(name, &matchList))
			{
			indexStr = name.GetSubstring(matchList.GetElement(2));
			while (indexStr.GetLength() < kThreadIndexWidth)
				{
				indexStr.PrependCharacter('0');
				}
			indexStr += ":  ";

			name.RemoveSubstring(1, matchList.GetFirstElement().last);
			name.TrimWhitespace();
			name.Prepend(indexStr);
			}

		CMThreadNode* node = new CMThreadNode(t.GetThreadID(), name, fileName, lineIndex);
		assert( node != NULL );

		root->Append(node);
		}

	GetWidget()->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
