/******************************************************************************
 JNamedTreeList.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JNamedTreeList
#define _H_JNamedTreeList

#include "JTreeList.h"

class JString;
class JNamedTreeNode;

class JNamedTreeList : public JTreeList
{
public:

	JNamedTreeList(JTree* tree);

	~JNamedTreeList() override;

	JNamedTreeNode*			GetNamedNode(const JIndex index);
	const JNamedTreeNode*	GetNamedNode(const JIndex index) const;

	const JString&	GetNodeName(const JIndex index) const;
	void			SetNodeName(const JIndex index, const JString& name);

	bool	Find(const JString& name, JIndex* index) const;
	bool	ClosestMatch(const JString& prefixStr, JIndex* index) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<JTreeNode>*	itsSortedNodeList;		// contents owned by tree

private:

	void	BuildSortedNodeList();
	void	CalledByBuildSortedNodeList(JTreeNode* node);
};

#endif
