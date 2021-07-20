/******************************************************************************
 JNamedTreeNode.cpp

	JTreeNode that includes a name.

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#include "JNamedTreeNode.h"
#include "JTree.h"
#include "JPtrArray-JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

	tree can be nullptr.  By not providing a constructor that takes a parent,
	we allow tree to be nullptr, and we allow JNamedTreeList to keep a sorted
	list of nodes.

 ******************************************************************************/

JNamedTreeNode::JNamedTreeNode
	(
	JTree*			tree,
	const JString&	name,
	const bool	isOpenable
	)
	:
	JTreeNode(tree, isOpenable),
	itsName(name)
{
	SetChildCompareFunction(DynamicCastCompareNames, JListT::kSortAscending, false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNamedTreeNode::~JNamedTreeNode()
{
}

/******************************************************************************
 SetName

 ******************************************************************************/

void
JNamedTreeNode::SetName
	(
	const JString& name
	)
{
	if (name != itsName)
		{
		itsName = name;

		NameChanged();

		JTree* tree;
		if (GetTree(&tree))
			{
			tree->BroadcastChange(this);
			}
		}
}

/******************************************************************************
 NameChanged (virtual protected)

	Derived classes can override this to update themselves.  This provides
	a more efficient way than listening to itsTree (which may not exist!).

 ******************************************************************************/

void
JNamedTreeNode::NameChanged()
{
}

/******************************************************************************
 FindNamedChild

 ******************************************************************************/

bool
JNamedTreeNode::FindNamedChild
	(
	const JString&		name,
	JNamedTreeNode**	node
	)
{
	const JNamedTreeNode* n;
	const bool found = FindNamedChild(name, &n);
	if (found)
		{
		*node = const_cast<JNamedTreeNode*>(n);
		}
	return found;
}

bool
JNamedTreeNode::FindNamedChild
	(
	const JString&			name,
	const JNamedTreeNode**	node
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		*node = GetNamedChild(i);
		if ((**node).GetName() == name)
			{
			return true;
			}
		}

	*node = nullptr;
	return false;
}

/******************************************************************************
 GetNamedParent

 ******************************************************************************/

JNamedTreeNode*
JNamedTreeNode::GetNamedParent()
{
	auto* node = dynamic_cast<JNamedTreeNode*>(GetParent());
	assert (node != nullptr);
	return node;
}

const JNamedTreeNode*
JNamedTreeNode::GetNamedParent()
	const
{
	const auto* node = dynamic_cast<const JNamedTreeNode*>(GetParent());
	assert (node != nullptr);
	return node;
}

bool
JNamedTreeNode::GetNamedParent
	(
	JNamedTreeNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<JNamedTreeNode*>(p);
		assert( *parent != nullptr );
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

bool
JNamedTreeNode::GetNamedParent
	(
	const JNamedTreeNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<const JNamedTreeNode*>(p);
		assert( *parent != nullptr );
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

/******************************************************************************
 GetNamedChild

 ******************************************************************************/

JNamedTreeNode*
JNamedTreeNode::GetNamedChild
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<JNamedTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const JNamedTreeNode*
JNamedTreeNode::GetNamedChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const JNamedTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

/******************************************************************************
 DynamicCastCompareNames (static protected)

 ******************************************************************************/

JListT::CompareResult
JNamedTreeNode::DynamicCastCompareNames
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	return CompareNames(dynamic_cast<JNamedTreeNode*>(e1),
						dynamic_cast<JNamedTreeNode*>(e2));
}

/******************************************************************************
 CompareNames (static)

 ******************************************************************************/

JListT::CompareResult
JNamedTreeNode::CompareNames
	(
	JNamedTreeNode * const & e1,
	JNamedTreeNode * const & e2
	)
{
	JListT::CompareResult result =
		JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
	if (result == JListT::kFirstEqualSecond)
		{
		result = JCompareStringsCaseSensitive(&(e1->itsName), &(e2->itsName));
		}
	return result;
}

/******************************************************************************
 DynamicCastCompareNamesForIncrSearch (static protected)

 ******************************************************************************/

JListT::CompareResult
JNamedTreeNode::DynamicCastCompareNamesForIncrSearch
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	return CompareNamesForIncrSearch(dynamic_cast<JNamedTreeNode*>(e1),
									 dynamic_cast<JNamedTreeNode*>(e2));
}

/******************************************************************************
 CompareNamesForIncrSearch (static)

 ******************************************************************************/

JListT::CompareResult
JNamedTreeNode::CompareNamesForIncrSearch
	(
	JNamedTreeNode * const & e1,
	JNamedTreeNode * const & e2
	)
{
	return JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
}
