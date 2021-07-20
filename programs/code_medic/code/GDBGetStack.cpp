/******************************************************************************
 GDBGetStack.cpp

	BASE CLASS = CMGetStack

	Copyright (C) 2001-09 by John Lindal.

 ******************************************************************************/

#include "GDBGetStack.h"
#include "GDBGetStackArguments.h"
#include "CMStackFrameNode.h"
#include "CMStackWidget.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStack::GDBGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
	:
	CMGetStack(JString("-stack-list-frames", JString::kNoCopy), tree, widget)
{
	itsArgsCmd = jnew GDBGetStackArguments(tree);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStack::~GDBGetStack()
{
	jdelete itsArgsCmd;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern  = "\\bframe=\\{";
static const JRegex assertPattern = "^(JAssert|__assert(_[^[:space:]]+)?)$";

void
GDBGetStack::HandleSuccess
	(
	const JString& cmdData
	)
{
	JTreeNode* root       = GetTree()->GetRoot();
	JIndex initFrameIndex = 0;

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetBytes());

	JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
	JString frameName, fileName;
	bool selectNextFrame = false;

	JStringIterator iter(data);
	while (iter.Next(framePattern))
		{
		stream.seekg(iter.GetLastMatch().GetUtf8ByteRange().last);
		if (!GDBLink::ParseMap(stream, &map))
			{
			CMGetLink()->Log("invalid data map");
			break;
			}
		iter.MoveTo(kJIteratorStartAfter, (std::streamoff) stream.tellg());

		JString* s;
		JIndex frameIndex;
		if (!map.GetElement("level", &s))
			{
			CMGetLink()->Log("missing frame index");
			continue;
			}
		if (!s->ConvertToUInt(&frameIndex))
			{
			CMGetLink()->Log("frame index is not integer");
			continue;
			}

		frameName = *s;
		while (frameName.GetCharacterCount() < kFrameIndexWidth)
			{
			frameName.Prepend("0");
			}
		frameName += ":  ";

		JString* fnName;
		if (!map.GetElement("func", &fnName))
			{
			CMGetLink()->Log("missing function name");
			continue;
			}
		frameName += *fnName;

		if (map.GetElement("file", &s))
			{
			fileName = *s;
			}

		JIndex lineIndex = 0;
		if (map.GetElement("line", &s) &&
			!s->ConvertToUInt(&lineIndex))
			{
			CMGetLink()->Log("line number is not integer");
			continue;
			}

		auto* node =
			jnew CMStackFrameNode(root, frameIndex, frameName,
								  fileName, lineIndex);
		assert( node != nullptr );
		root->Prepend(node);

		if (selectNextFrame)
			{
			initFrameIndex  = frameIndex;
			selectNextFrame = false;
			}
		else if (assertPattern.Match(*fnName))
			{
			selectNextFrame = true;
			}
		}

	itsArgsCmd->Send();

	GetWidget()->FinishedLoading(initFrameIndex);
}
