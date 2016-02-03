/******************************************************************************
 LLDBVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBVarCommand.h"
#include "CMVarNode.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"
#include "LLDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBVarCommand::LLDBVarCommand
	(
	const JCharacter* expr
	)
	:
	CMVarCommand()
{
	itsExpr = expr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBVarCommand::~LLDBVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBVarCommand::HandleSuccess
	(
	const JString& data
	)
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		Broadcast(ValueMessage(kValueFailed, NULL));
		return;
		}

	lldb::SBFrame f = link->GetDebugger()->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (!f.IsValid())
		{
		Broadcast(ValueMessage(kValueFailed, NULL));
		return;
		}

	lldb::SBValue v = f.EvaluateExpression(itsExpr, lldb::eDynamicDontRunTarget);

	if (v.IsValid() && v.GetValue() != NULL)
		{
//		BuildTree(f, 1, itsExpr, itsRootNode);

		CMVarNode* node = (CMGetLink())->CreateVarNode(NULL, v.GetName(), NULL, v.GetValue());
		assert( node != NULL );

		if (v.TypeIsPointerType())
			{
			node->MakePointer(kJTrue);
			}

		if (v.MightHaveChildren())
			{
			const JSize count = v.GetNumChildren();
			}

		Broadcast(ValueMessage(kValueUpdated, node));
		}
	else
		{
		Broadcast(ValueMessage(kValueFailed, NULL));
		}
}

/******************************************************************************
 BuildTree (static)

 ******************************************************************************/

void
LLDBVarCommand::BuildTree
	(
	lldb::SBFrame&		frame,
	lldb::SBValue&		value,
	const JCharacter*	expr,
	CMVarNode*			varRoot
	)
{
}

/******************************************************************************
 BuildTree (static private)

 ******************************************************************************/

void
LLDBVarCommand::BuildTree
	(
	const JSize			depth,
	lldb::SBFrame&		frame,
	const JCharacter*	expr,
	CMVarNode*			varRoot
	)
{
/*
	xmlNode* node = root->children;
	JString name, fullName, type, value;
	while (node != NULL)
		{
		name     = JGetXMLNodeAttr(node, (depth == 1 ? "fullname" : "name"));
		fullName = JGetXMLNodeAttr(node, "fullname");
		type     = JGetXMLNodeAttr(node, "type");

		CMVarNode* n;
		if (node->children != NULL && node->children->type == XML_TEXT_NODE)
			{
			value = (char*) node->children->content;

			const JString encoding = JGetXMLNodeAttr(node, "encoding");
			if (encoding == "base64")
				{
				value.DecodeBase64(&value);
				}

			if (type == "string")
				{
				for (JIndex i=value.GetLength(); i>=1; i--)
					{
					const JCharacter c = value.GetCharacter(i);
					if (c == '"' || c == '\\')
						{
						value.InsertCharacter('\\', i);
						}
					}

				value.PrependCharacter('"');
				value.AppendCharacter('"');
				}

			n = (CMGetLink())->CreateVarNode(NULL, name, fullName, value);
			assert( n != NULL );
			}
		else
			{
			n = (CMGetLink())->CreateVarNode(NULL, name, fullName, "");
			assert( n != NULL );

			if (type == "array" && JGetXMLNodeAttr(node, "children") == "1")
				{
				n->MakePointer(kJTrue);
				}

			if (node->children != NULL && node->children->type == XML_ELEMENT_NODE)
				{
				BuildTree(depth+1, node, n);
				}
			}

		varRoot->Append(n);	// avoid automatic update

		node = node->next;
		}
*/
}
