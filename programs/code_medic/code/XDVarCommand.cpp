/******************************************************************************
 XDVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

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
	const JString& cmd
	)
	:
	CMVarCommand()
{
	SetCommand(cmd);

	itsRootNode = CMGetLink()->CreateVarNode(false);
	assert( itsRootNode != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDVarCommand::~XDVarCommand()
{
	jdelete itsRootNode;
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
	auto* link = dynamic_cast<XDLink*>(CMGetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
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
		SetData(JString("<error reading value>", JString::kNoCopy));
		Broadcast(ValueMessage(kValueFailed, nullptr));
		}
	itsRootNode->DeleteAllChildren();
}
