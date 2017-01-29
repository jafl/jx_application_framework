/******************************************************************************
 JFSFileTreeNode.h

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

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

	virtual ~JFSFileTreeNode();

	JError				GoUp();
	JError				GoTo(const JCharacter* path);
	JError				Rename(const JCharacter* newName,
							   const JBoolean sort = kJTrue);
	virtual JBoolean	Update(const JBoolean force = kJFalse,
							   JFSFileTreeNodeBase** updateNode = NULL);
	void				UpdatePath(const Message& message);
	void				UpdatePath(const JString& oldPath, const JString& newPath);

	JDirEntry*			GetDirEntry();
	const JDirEntry*	GetDirEntry() const;

	virtual JBoolean	GetDirInfo(JDirInfo** info);
	virtual JBoolean	GetDirInfo(const JDirInfo** info) const;

	JBoolean		CanHaveChildren() const;
	static JBoolean	CanHaveChildren(const JDirEntry& entry);
	static JBoolean	CanHaveChildren(const JCharacter* path);

	JFSFileTreeNode*		GetFSChild(const JIndex index);
	const JFSFileTreeNode*	GetFSChild(const JIndex index) const;

protected:

	virtual JBoolean			OKToOpen() const;
	virtual JFSFileTreeNode*	CreateChild(JDirEntry* entry);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
	JDirInfo*	itsDirInfo;		// can be NULL

private:

	void		BuildChildList();
	JBoolean	CreateDirInfo();
	void		UpdateDirInfoSettings(const JDirInfo& info);

	void		UpdateChildren();
	void		UpdateAfterGo();
	void		UpdatePath(const JCharacter* fullName,
						   const JString& oldPath, const JString& newPath);

	static JListT::CompareResult
		CompareTypeAndName(JTreeNode * const & e1, JTreeNode * const & e2);

	// not allowed

	JFSFileTreeNode(const JFSFileTreeNode& source);
	const JFSFileTreeNode& operator=(const JFSFileTreeNode& source);
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
