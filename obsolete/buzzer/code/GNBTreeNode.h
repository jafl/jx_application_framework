/******************************************************************************
 GAddressBaseTreeNode.h

	Interface for GAddressBaseTreeNode class.

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GAddressBaseTreeNode
#define _H_GAddressBaseTreeNode

#include "JNamedTreeNode.h"
#include <iostream.h>

class JString;

class GAddressBaseTreeNode : public JNamedTreeNode
{
public:

	GAddressBaseTreeNode(JTree*	tree,
						JNamedTreeNode* parent = NULL,
						const char* name = "BASE",
						const JBoolean isBranch = kJTrue);

	virtual ~GAddressBaseTreeNode();

	virtual JString	GetFullPathAndName() const = 0;
	virtual void	Save(ostream& os);

private:

	// not allowed

	GAddressBaseTreeNode(const GAddressBaseTreeNode& source);
	const GAddressBaseTreeNode& operator=(const GAddressBaseTreeNode& source);

};

#endif
