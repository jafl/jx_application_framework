/******************************************************************************
 XDGetStack.cpp

	BASE CLASS = CMGetStack

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XDGetStack.h"
#include "CMStackFrameNode.h"
#include "CMStackWidget.h"
#include "XDLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <jFileUtil.h>
#include <jAssert.h>

const JSize kFrameIndexWidth = 2;	// width of frame index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetStack::XDGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
	:
	CMGetStack("stack_get", tree, widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetStack::~XDGetStack()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetStack::HandleSuccess
	(
	const JString& data
	)
{
	XDLink* link = dynamic_cast<XDLink*>(CMGetLink());
	xmlNode* root;
	if (link == NULL || !link->GetParsedData(&root))
		{
		return;
		}

	JTreeNode* stackRoot = GetTree()->GetRoot();

	xmlNode* frame  = root->children;
	JString frameIndexStr, fileName, lineStr, frameName, path, name;
	while (frame != NULL)
		{
		frameIndexStr = JGetXMLNodeAttr(frame, "level");
		JUInt frameIndex;
		JBoolean ok = frameIndexStr.ConvertToUInt(&frameIndex);
		assert( ok );

		fileName = JGetXMLNodeAttr(frame, "filename");

		lineStr = JGetXMLNodeAttr(frame, "lineno");
		JUInt lineIndex;
		ok = lineStr.ConvertToUInt(&lineIndex);
		assert( ok );

		name = JGetXMLNodeAttr(frame, "where");
		if (name.IsEmpty())
			{
			JSplitPathAndName(fileName, &path, &name);
			}

		frameName = frameIndexStr;
		while (frameName.GetLength() < kFrameIndexWidth)
			{
			frameName.PrependCharacter('0');
			}
		frameName += ":  ";
		frameName += name;

		CMStackFrameNode* node =
			new CMStackFrameNode(stackRoot, frameIndex, frameName,
								 fileName, lineIndex);
		assert( node != NULL );
		stackRoot->Prepend(node);

		frame = frame->next;
		}

	GetWidget()->FinishedLoading(0);
	GetWidget()->SwitchToFrame(0);
}
