/******************************************************************************
 GDBGetStackArguments.cpp

	BASE CLASS = CMCommand

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetStackArguments.h"
#include "CMStackFrameNode.h"
#include "CMStackArgNode.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStackArguments::GDBGetStackArguments
	(
	JTree* tree
	)
	:
	CMCommand("-stack-list-arguments 1", kJFalse, kJTrue),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStackArguments::~GDBGetStackArguments()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex framePattern =
	"frame=\\{"
	"level=\"(?P<FrameIndex>[[:digit:]]+)\""
	",args=\\[";

static const JRegex refPattern = "@0x[[:xdigit:]]+:";

void
GDBGetStackArguments::HandleSuccess
	(
	const JString& cmdData
	)
{
	JTreeNode* root        = itsTree->GetRoot();
	const JSize frameCount = root->GetChildCount();
	if (frameCount == 0)
		{
		return;
		}

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetCString());

	JIndexRange matchedRange, refRange;
	JArray<JIndexRange> matchList;
	JString frameIndexStr;
	JPtrArray< JStringPtrMap<JString> > argList(JPtrArrayT::kDeleteAll);
	while (framePattern.MatchAfter(data, matchedRange, &matchList))
		{
		matchedRange = matchList.GetFirstElement();
		if (data.GetCharacter(matchedRange.last+1) == ']')
			{
			continue;
			}

		framePattern.GetSubexpression(data, "FrameIndex", matchList, &frameIndexStr);
		JIndex frameIndex;
		JBoolean ok = frameIndexStr.ConvertToUInt(&frameIndex);
		assert( ok );

		CMStackFrameNode* frameNode =
			dynamic_cast<CMStackFrameNode*>(root->GetChild(frameCount - frameIndex));
		assert( frameNode != NULL );

		stream.seekg(matchedRange.last);
		if (!GDBLink::ParseMapArray(stream, &argList))
			{
			(CMGetLink())->Log("invalid stack argument list");
			break;
			}

		const JSize count = argList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JStringPtrMap<JString>* arg = argList.NthElement(i);
			JString *name, *value;
			if (!arg->GetElement("name", &name) ||
				!arg->GetElement("value", &value))
				{
				(CMGetLink())->Log("invalid stack argument");
				continue;
				}

			if (refPattern.Match(*value, &refRange))
				{
				value->RemoveSubstring(refRange.last, value->GetLength());
				}

			CMStackArgNode* argNode = new CMStackArgNode(frameNode, *name, *value);
			assert( argNode != NULL );
			}
		}
}
