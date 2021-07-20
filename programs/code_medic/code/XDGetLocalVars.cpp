/******************************************************************************
 XDGetLocalVars.cpp

	BASE CLASS = CMGetLocalVars

	Copyright (C) 2007 by John Lindal.

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
	CMGetLocalVars(JString("context_names", JString::kNoCopy)),
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
	auto* link = dynamic_cast<XDLink*>(CMGetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
		{
		return;
		}

	itsRootNode->DeleteAllChildren();

	xmlNode* node = root->children;
	JString id;
	while (node != nullptr)
		{
		if (JGetXMLNodeAttr(node, "name") != "Globals")
			{
			id = JGetXMLNodeAttr(node, "id");

			auto* cmd = jnew XDGetContextVars(itsRootNode, id);
			assert( cmd != nullptr );
			cmd->Send();
			}

		node = node->next;
		}
}
