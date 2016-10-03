/******************************************************************************
 XDGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XDGetLocalVars.h"
#include "XDGetContextVars.h"
#include "XDLink.h"
#include "CMVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetLocalVars::XDGetLocalVars
	(
	CMVarNode* rootNode
	)
	:
	CMGetLocalVars("context_names"),
	itsRootNode(rootNode)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetLocalVars::~XDGetLocalVars()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetLocalVars::HandleSuccess
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

	itsRootNode->DeleteAllChildren();

	xmlNode* node = root->children;
	JString id;
	while (node != NULL)
		{
		if (JGetXMLNodeAttr(node, "name") != "Globals")
			{
			id = JGetXMLNodeAttr(node, "id");

			XDGetContextVars* cmd = new XDGetContextVars(itsRootNode, id);
			assert( cmd != NULL );
			cmd->Send();
			}

		node = node->next;
		}
}
