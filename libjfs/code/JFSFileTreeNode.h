/******************************************************************************
 JFSFileTreeNode.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JFSFileTreeNode
#define _H_JFSFileTreeNode

#include "jx-af/jfs/JFSFileTreeNodeBase.h"

class JError;
class JDirInfo;
class JDirEntry;

class JFSFileTreeNode : public JFSFileTreeNodeBase
{
public:

	JFSFileTreeNode(JDirEntry* entry);

	virtual ~JFSFileTreeNode();

	JError				GoUp();
	JError				GoTo(const JString& path);
	JError				Rename(const JString& newName,
							   const bool sort = true);
	virtual bool	Update(const bool force = false,
							   JFSFileTreeNodeBase** updateNode = nullptr) override;
	void				UpdatePath(const Message& message);
	void				UpdatePath(const JString& oldPath, const JString& newPath);

	JDirEntry*			GetDirEntry();
	const JDirEntry*	GetDirEntry() const;

	virtual bool	GetDirInfo(JDirInfo** info) override;
	virtual bool	GetDirInfo(const JDirInfo** info) const override;

	bool		CanHaveChildren() const;
	static bool	CanHaveChildren(const JDirEntry& entry);
	static bool	CanHaveChildren(const JString& path);

	JFSFileTreeNode*		GetFSChild(const JIndex index);
	const JFSFileTreeNode*	GetFSChild(const JIndex index) const;

protected:

	virtual bool			OKToOpen() const override;
	virtual JFSFileTreeNode*	CreateChild(JDirEntry* entry);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	static JListT::CompareResult
		CompareUserName(JTreeNode * const & e1, JTreeNode * const & e2);

	static JListT::CompareResult
		CompareGroupName(JTreeNode * const & e1, JTreeNode * const & e2);

	static JListT::CompareResult
		CompareSize(JTreeNode * const & e1, JTreeNode * const & e2);

	static JListT::CompareResult
		CompareDate(JTreeNode * const & e1, JTreeNode * const & e2);

private:

	JDirEntry*	itsDirEntry;
	JDirInfo*	itsDirInfo;		// can be nullptr

private:

	void		BuildChildList();
	bool	CreateDirInfo();
	void		UpdateDirInfoSettings(const JDirInfo& info);

	void		UpdateChildren();
	void		UpdateAfterGo();
	void		UpdatePath(const JString& fullName,
						   const JString& oldPath, const JString& newPath);

	static JListT::CompareResult
		CompareTypeAndName(JTreeNode * const & e1, JTreeNode * const & e2);
};

/******************************************************************************
 GetDirEntry

 ******************************************************************************/

inline JDirEntry*
JFSFileTreeNode::GetDirEntry()
{
	return itsDirEntry;
}

inline const JDirEntry*
JFSFileTreeNode::GetDirEntry()
	const
{
	return itsDirEntry;
}

#endif
