/******************************************************************************
 JNamedTreeNode.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JNamedTreeNode
#define _H_JNamedTreeNode

#include "JTreeNode.h"
#include "JString.h"

class JNamedTreeList;

class JNamedTreeNode : public JTreeNode
{
	friend class JNamedTreeList;

public:

	JNamedTreeNode(JTree* tree, const JString& name,
				   const bool isOpenable = true);

	virtual	~JNamedTreeNode();

	const JString&	GetName() const;
	void			SetName(const JString& name);

	bool		FindNamedChild(const JString& name,
								   JNamedTreeNode** node);
	bool		FindNamedChild(const JString& name,
								   const JNamedTreeNode** node) const;

	JNamedTreeNode*			GetNamedParent();
	const JNamedTreeNode*	GetNamedParent() const;
	bool				GetNamedParent(JNamedTreeNode** parent);
	bool				GetNamedParent(const JNamedTreeNode** parent) const;

	JNamedTreeNode*			GetNamedChild(const JIndex index);
	const JNamedTreeNode*	GetNamedChild(const JIndex index) const;

	static JListT::CompareResult
		CompareNames(JNamedTreeNode * const & e1, JNamedTreeNode * const & e2);
	static JListT::CompareResult
		CompareNamesForIncrSearch(JNamedTreeNode * const & e1, JNamedTreeNode * const & e2);

protected:

	virtual void	NameChanged();

	static JListT::CompareResult
		DynamicCastCompareNames(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		DynamicCastCompareNamesForIncrSearch(JTreeNode * const & e1, JTreeNode * const & e2);

private:

	JString	itsName;

private:

	// not allowed

	JNamedTreeNode(const JNamedTreeNode& source);
	const JNamedTreeNode& operator=(const JNamedTreeNode& source);
};


/******************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
JNamedTreeNode::GetName()
	const
{
	return itsName;
}

#endif
