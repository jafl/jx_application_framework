/******************************************************************************
 JFSFileTreeNodeBase.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFSFileTreeNodeBase
#define _H_JFSFileTreeNodeBase

#include <jx-af/jcore/JNamedTreeNode.h>

class JDirInfo;
class JFSFileTree;

class JFSFileTreeNodeBase : public JNamedTreeNode
{
public:

	JFSFileTreeNodeBase(const bool isOpenable);

	virtual ~JFSFileTreeNodeBase();

	virtual bool	Update(const bool force = false,
						   JFSFileTreeNodeBase** updateNode = nullptr);

	virtual bool	GetDirInfo(JDirInfo** info) = 0;
	virtual bool	GetDirInfo(const JDirInfo** info) const = 0;

	JFSFileTree*			GetFSFileTree();
	const JFSFileTree*		GetFSFileTree() const;

	JFSFileTreeNodeBase*		GetFSParent();
	const JFSFileTreeNodeBase*	GetFSParent() const;
	bool						GetFSParent(JFSFileTreeNodeBase** parent);
	bool						GetFSParent(const JFSFileTreeNodeBase** parent) const;
};

#endif
