/******************************************************************************
 GDBGetStack.cpp

	BASE CLASS = CMGetStack

	Copyright (C) 2001-09 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetStack.h"
#include "GDBGetStackArguments.h"
#include "CMStackFrameNode.h"
#include "CMStackWidget.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JTree.h>
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
	CMGetStack("-stack-list-frames", tree, widget)
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
	std::istringstream stream(data.GetCString());

	JIndexRange origRange, newRange;
	JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
	JString frameName, fileName;
	JBoolean selectNextFrame = kJFalse;
	while (framePattern.MatchAfter(data, origRange, &newRange))
		{
		stream.seekg(newRange.last);
		if (!GDBLink::ParseMap(stream, &map))
			{
			(CMGetLink())->Log("invalid data map");
			break;
			}
		origRange.first = origRange.last = ((std::streamoff) stream.tellg()) + 1;

		JString* s;
		JIndex frameIndex;
		if (!map.GetElement("level", &s))
			{
			(CMGetLink())->Log("missing frame index");
			continue;
			}
		if (!s->ConvertToUInt(&frameIndex))
			{
			(CMGetLink())->Log("frame index is not integer");
			continue;
			}

		frameName = *s;
		while (frameName.GetLength() < kFrameIndexWidth)
			{
			frameName.PrependCharacter('0');
			}
		frameName += ":  ";

		JString* fnName;
		if (!map.GetElement("func", &fnName))
			{
			(CMGetLink())->Log("missing function name");
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
			(CMGetLink())->Log("line number is not integer");
			continue;
			}

		CMStackFrameNode* node =
			jnew CMStackFrameNode(root, frameIndex, frameName,
								 fileName, lineIndex);
		assert( node != NULL );
		root->Prepend(node);

		if (selectNextFrame)
			{
			initFrameIndex  = frameIndex;
			selectNextFrame = kJFalse;
			}
		else if (assertPattern.Match(*fnName))
			{
			selectNextFrame = kJTrue;
			}
		}

	itsArgsCmd->Send();

	GetWidget()->FinishedLoading(initFrameIndex);
}
