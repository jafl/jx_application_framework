/******************************************************************************
 JNamedTreeList.h

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNamedTreeList
#define _H_JNamedTreeList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTreeList.h>

class JString;
class JNamedTreeNode;

class JNamedTreeList : public JTreeList
{
public:

	JNamedTreeList(JTree* tree);

	virtual	~JNamedTreeList();

	JNamedTreeNode*			GetNamedNode(const JIndex index);
	const JNamedTreeNode*	GetNamedNode(const JIndex index) const;

	const JString&	GetNodeName(const JIndex index) const;
	void			SetNodeName(const JIndex index, const JCharacter* name);

	JBoolean	Find(const JCharacter* name, JIndex* index) const;
	JBoolean	ClosestMatch(const JCharacter* prefixStr, JIndex* index) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JPtrArray<JTreeNode>*	itsSortedNodeList;		// contents owned by tree

private:

	void	BuildSortedNodeList();
	void	BuildSortedNodeList1(JTreeNode* node);

	// not allowed

	JNamedTreeList(const JNamedTreeList& source);
	const JNamedTreeList& operator=(const JNamedTreeList& source);
};

#endif
