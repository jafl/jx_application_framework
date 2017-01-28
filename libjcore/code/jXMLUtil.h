/******************************************************************************
 jXMLUtil.h

   Copyright (C) 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_jXMLUtil
#define _H_jXMLUtil

#include "JString.h"
#include <libxml/tree.h>

xmlNode*	JGetXMLChildNode(xmlNode* node, const JUtf8Byte* childName);
JString		JGetXMLNodeAttr(xmlNode* node, const JUtf8Byte* attrName);

#endif
