/******************************************************************************
 jXMLUtils.h

   Copyright (C) 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_jXMLUtils
#define _H_jXMLUtils

#include "JString.h"
#include <libxml/tree.h>

xmlNode*	JGetXMLChildNode(xmlNode* node, const JCharacter* childName);
JString		JGetXMLNodeAttr(xmlNode* node, const JCharacter* attrName);

#endif
