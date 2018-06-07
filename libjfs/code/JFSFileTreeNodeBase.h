/******************************************************************************
 JFSFileTreeNodeBase.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFSFileTreeNodeBase
#define _H_JFSFileTreeNodeBase

#include <JNamedTreeNode.h>

class JDirInfo;
class JFSFileTree;

class JFSFileTreeNodeBase : public JNamedTreeNode
{
public:

	JFSFileTreeNodeBase(const JBoolean isOpenable);

	virtual ~JFSFileTreeNodeBase();

	virtual JBoolean	Update(const JBoolean force = kJFalse,
							   JFSFileTreeNodeBase** updateNode = nullptr);

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
