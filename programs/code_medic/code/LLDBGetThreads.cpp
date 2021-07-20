/******************************************************************************
 LLDBGetThreads.cpp

	BASE CLASS = CMGetThreads

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetThreads.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBStream.h"
#include "CMThreadsWidget.h"
#include "CMThreadNode.h"
#include "cmGlobals.h"
#include "LLDBLink.h"	// must be after X11 includes: Status
#include <JTree.h>
#include <JStringIterator.h>
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
	CMGetThreads(JString::empty, widget),
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

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = p.GetNumThreads();
	JString fileName, name, indexStr;
	for (JUnsignedOffset i=0; i<count; i++)
		{
		lldb::SBThread t = p.GetThreadAtIndex(i);
		lldb::SBFrame f  = t.GetSelectedFrame();

		JIndex lineIndex      = 0;
		lldb::SBLineEntry e   = f.GetLineEntry();
		lldb::SBFileSpec file = e.GetFileSpec();
		if (file.IsValid())
			{
			fileName = JCombinePathAndName(
				JString(file.GetDirectory(), JString::kNoCopy),
				JString(file.GetFilename(), JString::kNoCopy));

			lineIndex = e.GetLine();
			}
		else
			{
			fileName.Clear();
			}

		lldb::SBStream stream;
		t.GetDescription(stream);
		name = stream.GetData();

		const JStringMatch m = threadIDPattern.Match(name, JRegex::kIncludeSubmatches);
		if (!m.IsEmpty())
			{
			indexStr = m.GetSubstring(1);
			while (indexStr.GetCharacterCount() < kThreadIndexWidth)
				{
				indexStr.Prepend("0");
				}
			indexStr += ":  ";

			JStringIterator iter(&name);
			iter.RemoveNext(m.GetCharacterRange().last);
			name.TrimWhitespace();
			name.Prepend(indexStr);
			}

		auto* node = jnew CMThreadNode(t.GetThreadID(), name, fileName, lineIndex);
		assert( node != nullptr );

		root->Append(node);
		}

	GetWidget()->FinishedLoading(p.GetSelectedThread().GetThreadID());
}
