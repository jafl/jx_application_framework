/******************************************************************************
 XDGetContextVars.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XDGetContextVars.h"
#include "XDLink.h"
#include "CMVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetContextVars::XDGetContextVars
	(
	CMVarNode*			rootNode,
	const JCharacter*	contextID
	)
	:
	CMCommand("", kJTrue, kJTrue),
	itsRootNode(rootNode)
{
	JString cmd = "context_get -c ";
	cmd        += contextID;
	cmd        += " -d ";
	cmd        += dynamic_cast<XDLink*>(CMGetLink())->GetStackFrameIndex();
	SetCommand(cmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetContextVars::~XDGetContextVars()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetContextVars::HandleSuccess
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

	BuildTree(1, root, itsRootNode);
}

/******************************************************************************
 BuildTree (static)

 ******************************************************************************/

void
XDGetContextVars::BuildTree
	(
	const JSize	depth,
	xmlNode*	root,
	CMVarNode*	varRoot
	)
{
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
}
