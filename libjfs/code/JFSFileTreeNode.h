/******************************************************************************
 JFSFileTreeNode.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JFSFileTreeNode
#define _H_JFSFileTreeNode

#include "JFSFileTreeNodeBase.h"

class JError;
class JDirInfo;
class JDirEntry;

class JFSFileTreeNode : public JFSFileTreeNodeBase
{
public:

	JFSFileTreeNode(JDirEntry* entry);

	~JFSFileTreeNode() override;

	JError	GoUp();
	JError	GoTo(const JString& path);
	JError	Rename(const JString& newName, const bool sort = true);
	bool	Update(const bool force = false,
				   JFSFileTreeNodeBase** updateNode = nullptr) override;
	void	UpdatePath(const Message& message);
	void	UpdatePath(const JString& oldPath, const JString& newPath);

	JDirEntry*			GetDirEntry();
	const JDirEntry*	GetDirEntry() const;

	bool	GetDirInfo(JDirInfo** info) override;
	bool	GetDirInfo(const JDirInfo** info) const override;

	bool		CanHaveChildren() const;
	static bool	CanHaveChildren(const JDirEntry& entry);
	static bool	CanHaveChildren(const JString& path);

	JFSFileTreeNode*		GetFSChild(const JIndex index);
	const JFSFileTreeNode*	GetFSChild(const JIndex index) const;

protected:

	bool						OKToOpen() const override;
	virtual JFSFileTreeNode*	CreateChild(JDirEntry* entry);

	void	Receive(JBroadcaster* sender, const Message& message) override;

	static std::weak_ordering
		CompareUserNames(JTreeNode * const & e1, JTreeNode * const & e2);

	static std::weak_ordering
		CompareGroupNames(JTreeNode * const & e1, JTreeNode * const & e2);

	static std::weak_ordering
		CompareSizes(JTreeNode * const & e1, JTreeNode * const & e2);

	static std::weak_ordering
		CompareDates(JTreeNode * const & e1, JTreeNode * const & e2);

private:

	JDirEntry*	itsDirEntry;
	JDirInfo*	itsDirInfo;		// can be nullptr

private:

	void	BuildChildList();
	bool	CreateDirInfo();
	void	UpdateDirInfoSettings(const JDirInfo& info);

	void	UpdateChildren();
	void	UpdateAfterGo();
	void	UpdatePath(const JString& fullName,
					   const JString& oldPath, const JString& newPath);

	static std::weak_ordering
		CompareTypesAndNames(JTreeNode * const & e1, JTreeNode * const & e2);
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
