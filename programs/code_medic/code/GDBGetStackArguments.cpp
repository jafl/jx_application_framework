/******************************************************************************
 GDBGetStackArguments.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "GDBGetStackArguments.h"
#include "CMStackFrameNode.h"
#include "CMStackArgNode.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <JStringIterator.h>
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
	CMCommand("-stack-list-arguments 1", false, true),
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
	std::istringstream stream(data.GetBytes());

	JPtrArray< JStringPtrMap<JString> > argList(JPtrArrayT::kDeleteAll);

	JStringIterator iter(data);
	JUtf8Character c;
	while (iter.Next(framePattern))
		{
		if (iter.Next(&c, kJIteratorStay) && c == ']')
			{
			continue;
			}

		JIndex frameIndex;
		bool ok = iter.GetLastMatch().GetSubstring("FrameIndex").ConvertToUInt(&frameIndex);
		assert( ok );

		auto* frameNode =
			dynamic_cast<CMStackFrameNode*>(root->GetChild(frameCount - frameIndex));
		assert( frameNode != nullptr );

		stream.seekg(iter.GetLastMatch().GetUtf8ByteRange().last);
		if (!GDBLink::ParseMapArray(stream, &argList))
			{
			CMGetLink()->Log("invalid stack argument list");
			break;
			}

		const JSize count = argList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JStringPtrMap<JString>* arg = argList.GetElement(i);
			JString *name, *value;
			if (!arg->GetElement("name", &name) ||
				!arg->GetElement("value", &value))
				{
				CMGetLink()->Log("invalid stack argument");
				continue;
				}

			JStringIterator iter(value);
			if (iter.Next(refPattern))
				{
				iter.SkipPrev();
				iter.RemoveAllNext();
				}
			iter.Invalidate();

			auto* argNode = jnew CMStackArgNode(frameNode, *name, *value);
			assert( argNode != nullptr );
			}
		}
}
