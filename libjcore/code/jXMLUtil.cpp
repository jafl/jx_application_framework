/******************************************************************************
 jXMLUtil.cpp

	Utilities built on top of libxml2.

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "jXMLUtil.h"
#include "jAssert.h"

/******************************************************************************
 JGetXMLChildNode

 *****************************************************************************/

xmlNode*
JGetXMLChildNode
	(
	xmlNode*			node,
	const JUtf8Byte*	childName
	)
{
	xmlNode* child = node->children;
	while (child != nullptr)
{
		if (strcmp((char*) child->name, childName) == 0)
	{
			break;
	}
		child = child->next;
}

	return child;
}

/******************************************************************************
 JGetXMLNodeAttr

 *****************************************************************************/

JString
JGetXMLNodeAttr
	(
	xmlNode*			node,
	const JUtf8Byte*	attrName
	)
{
	JString value;

	xmlAttr* attr = node->properties;
	while (attr != nullptr)
{
		if (strcmp((char*) attr->name, attrName) == 0)
	{
			value = (char*) attr->children->content;
			break;
	}
		attr = attr->next;
}

	return value;
}
