/******************************************************************************
 JFSFileTreeNodeBase.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFSFileTreeNodeBase
#define _H_JFSFileTreeNodeBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNamedTreeNode.h>

class JDirInfo;
class JFSFileTree;

class JFSFileTreeNodeBase : public JNamedTreeNode
{
public:

	JFSFileTreeNodeBase(const JBoolean isOpenable);

	virtual ~JFSFileTreeNodeBase();

	virtual JBoolean	Update(const JBoolean force = kJFalse,
							   JFSFileTreeNodeBase** updateNode = NULL);

	virtual JBoolean	GetDirInfo(JDirInfo** info) = 0;
	virtual JBoolean	GetDirInfo(const JDirInfo** info) const = 0;

	JFSFileTree*			GetFSFileTree();
	const JFSFileTree*		GetFSFileTree() const;

	JFSFileTreeNodeBase*		GetFSParent();
	const JFSFileTreeNodeBase*	GetFSParent() const;
	JBoolean					GetFSParent(JFSFileTreeNodeBase** parent);
	JBoolean					GetFSParent(const JFSFileTreeNodeBase** parent) const;

private:

	// not allowed

	JFSFileTreeNodeBase(const JFSFileTreeNodeBase& source);
	const JFSFileTreeNodeBase& operator=(const JFSFileTreeNodeBase& source);
};

#endif
