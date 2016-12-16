/******************************************************************************
 GNBCategoryNode.h

	Interface for GNBCategoryNode class.

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBCategoryNode
#define _H_GNBCategoryNode

#include "GNBBaseNode.h"
#include <iostream.h>

class JString;
//class std::istream;
class JTree;

class GNBCategoryNode : public GNBBaseNode
{
public:

	GNBCategoryNode(JNamedTreeNode* parent = NULL, const char* name = "BASE");
	GNBCategoryNode(JTree* tree, const JCharacter* name);
	GNBCategoryNode(const GNBCategoryNode& source, JTree* tree);

	virtual ~GNBCategoryNode();

private:

	// not allowed

	GNBCategoryNode(const GNBCategoryNode& source);
	const GNBCategoryNode& operator=(const GNBCategoryNode& source);

};

#endif
