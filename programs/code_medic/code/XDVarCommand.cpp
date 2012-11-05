/******************************************************************************
 XDVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDVarCommand.h"
#include "XDGetContextVars.h"
#include "XDLink.h"
#include "CMVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDVarCommand::XDVarCommand
	(
	const JCharacter* cmd
	)
	:
	CMVarCommand()
{
	SetCommand(cmd);

	itsRootNode = (CMGetLink())->CreateVarNode(kJFalse);
	assert( itsRootNode != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDVarCommand::~XDVarCommand()
{
	delete itsRootNode;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDVarCommand::HandleSuccess
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

	XDGetContextVars::BuildTree(1, root, itsRootNode);
	if (itsRootNode->HasChildren())
		{
		Broadcast(ValueMessage(kValueUpdated, itsRootNode->GetVarChild(1)));
		}
	else
		{
		SetData("<error reading value>");
		Broadcast(ValueMessage(kValueFailed, NULL));
		}
	itsRootNode->DeleteAllChildren();
}
