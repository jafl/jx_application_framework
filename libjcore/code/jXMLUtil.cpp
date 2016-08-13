/******************************************************************************
 jXMLUtils.cc

	Utilities built on top of libxml2.

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "jXMLUtil.h"
#include <jAssert.h>

/******************************************************************************
 JGetXMLChildNode

 *****************************************************************************/

xmlNode*
JGetXMLChildNode
	(
	xmlNode*			node,
	const JCharacter*	childName
	)
{
	xmlNode* child = node->children;
	while (child != NULL)
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
	const JCharacter*	attrName
	)
{
	JString value;

	xmlAttr* attr = node->properties;
	while (attr != NULL)
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
