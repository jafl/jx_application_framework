/******************************************************************************
 SVNRepoTreeList.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNRepoTreeList
#define _H_SVNRepoTreeList

#include <JNamedTreeList.h>

class SVNRepoTree;
class SVNRepoTreeNode;

class SVNRepoTreeList : public JNamedTreeList
{
public:

	SVNRepoTreeList(SVNRepoTree* tree);

	virtual ~SVNRepoTreeList();

	SVNRepoTree*		GetRepoTree();
	const SVNRepoTree*	GetRepoTree() const;

	SVNRepoTreeNode*		GetRepoNode(const JIndex index);
	const SVNRepoTreeNode*	GetRepoNode(const JIndex index) const;

private:

	// not allowed

	SVNRepoTreeList(const SVNRepoTreeList& source);
	const SVNRepoTreeList& operator=(const SVNRepoTreeList& source);

};

#endif
