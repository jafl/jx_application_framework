/******************************************************************************
 GNBRootNode.h

	Interface for GNBRootNode class.

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBRootNode
#define _H_GNBRootNode

#include "GNBBaseNode.h"
#include <iostream.h>

class JString;

class GNBRootNode : public GNBBaseNode
{
public:

	GNBRootNode(JTree*	tree,
						const char* name = "BASE");

	virtual ~GNBRootNode();

private:

	// not allowed

	GNBRootNode(const GNBRootNode& source);
	const GNBRootNode& operator=(const GNBRootNode& source);

};

#endif
