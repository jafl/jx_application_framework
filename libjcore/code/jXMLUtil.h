/******************************************************************************
 jXMLUtils.h

   Copyright © 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_jXMLUtils
#define _H_jXMLUtils

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JString.h"
#include <libxml/tree.h>

xmlNode*	JGetXMLChildNode(xmlNode* node, const JCharacter* childName);
JString		JGetXMLNodeAttr(xmlNode* node, const JCharacter* attrName);

#endif
