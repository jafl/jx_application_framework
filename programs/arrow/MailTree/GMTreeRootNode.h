/******************************************************************************
 GMTreeRootNode.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMTreeRootNode
#define _H_GMTreeRootNode

#include <JFSFileTreeNodeBase.h>

class JDirInfo;

class GMTreeRootNode : public JFSFileTreeNodeBase
{
public:

	GMTreeRootNode();

	virtual ~GMTreeRootNode();

	virtual JBoolean	GetDirInfo(JDirInfo** info);
	virtual JBoolean	GetDirInfo(const JDirInfo** info) const;

private:

	JDirInfo* itsInfo;

private:

	// not allowed

	GMTreeRootNode(const GMTreeRootNode& source);
	const GMTreeRootNode& operator=(const GMTreeRootNode& source);
};

#endif
