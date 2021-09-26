/******************************************************************************
 jXMLUtil.h

   Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#ifndef _H_jXMLUtil
#define _H_jXMLUtil

#include "jx-af/jcore/JString.h"
#include <libxml/tree.h>

xmlNode*	JGetXMLChildNode(xmlNode* node, const JUtf8Byte* childName);
JString		JGetXMLNodeAttr(xmlNode* node, const JUtf8Byte* attrName);

#endif
