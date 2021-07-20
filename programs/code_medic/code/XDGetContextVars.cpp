/******************************************************************************
 XDGetContextVars.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetContextVars.h"
#include "XDLink.h"
#include "CMVarNode.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetContextVars::XDGetContextVars
	(
	CMVarNode*		rootNode,
	const JString&	contextID
	)
	:
	CMCommand("", true, true),
	itsRootNode(rootNode)
{
	JString cmd("context_get -c ");
	cmd += contextID;
	cmd += " -d ";
	cmd += JString((JUInt64) dynamic_cast<XDLink*>(CMGetLink())->GetStackFrameIndex());
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
	auto* link = dynamic_cast<XDLink*>(CMGetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
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
	while (node != nullptr)
		{
		name     = JGetXMLNodeAttr(node, (depth == 1 ? "fullname" : "name"));
		fullName = JGetXMLNodeAttr(node, "fullname");
		type     = JGetXMLNodeAttr(node, "type");

		CMVarNode* n;
		if (node->children != nullptr && node->children->type == XML_TEXT_NODE)
			{
			value = (char*) node->children->content;

			const JString encoding = JGetXMLNodeAttr(node, "encoding");
			if (encoding == "base64")
				{
				value.DecodeBase64(&value);
				}

			if (type == "string")
				{
				JStringIterator iter(&value, kJIteratorStartAtEnd);
				JUtf8Character c;
				while (iter.Prev(&c))
					{
					if (c == '"' || c == '\\')
						{
						iter.Insert("\\");
						}
					}

				value.Prepend("\"");
				value.Append("\"");
				}

			n = CMGetLink()->CreateVarNode(nullptr, name, fullName, value);
			assert( n != nullptr );
			}
		else
			{
			n = CMGetLink()->CreateVarNode(nullptr, name, fullName, JString::empty);
			assert( n != nullptr );

			if (type == "array" && JGetXMLNodeAttr(node, "children") == "1")
				{
				n->MakePointer(true);
				}

			if (node->children != nullptr && node->children->type == XML_ELEMENT_NODE)
				{
				BuildTree(depth+1, node, n);
				}
			}

		varRoot->Append(n);	// avoid automatic update

		node = node->next;
		}
}
