/******************************************************************************
 SyGFileTreeList.h

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFileTreeList
#define _H_SyGFileTreeList

#include <JNamedTreeList.h>

class JDirEntry;
class SyGFileTree;
class SyGFileTreeNode;

class SyGFileTreeList : public JNamedTreeList
{
public:

	SyGFileTreeList(SyGFileTree* tree);

	virtual ~SyGFileTreeList();

	SyGFileTree*		GetSyGFileTree();
	const SyGFileTree*	GetSyGFileTree() const;

	SyGFileTreeNode*		GetSyGNode(const JIndex index);
	const SyGFileTreeNode*	GetSyGNode(const JIndex index) const;

//	JDirEntry*			GetDirEntry(const JIndex index);
	const JDirEntry*	GetDirEntry(const JIndex index) const;

protected:

	virtual JBoolean	ShouldOpenSibling(const JTreeNode* node);
	virtual JBoolean	ShouldOpenDescendant(const JTreeNode* node);

private:

	// not allowed

	SyGFileTreeList(const SyGFileTreeList& source);
	const SyGFileTreeList& operator=(const SyGFileTreeList& source);

};

#endif
