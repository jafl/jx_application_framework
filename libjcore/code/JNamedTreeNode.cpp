/******************************************************************************
 JNamedTreeNode.cpp

	JTreeNode that includes a name.

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JNamedTreeNode.h>
#include <JTree.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	tree can be NULL.  By not providing a constructor that takes a parent,
	we allow tree to be NULL, and we allow JNamedTreeList to keep a sorted
	list of nodes.

 ******************************************************************************/

JNamedTreeNode::JNamedTreeNode
	(
	JTree*				tree,
	const JCharacter*	name,
	const JBoolean		isOpenable
	)
	:
	JTreeNode(tree, isOpenable),
	itsName(name)
{
	SetChildCompareFunction(DynamicCastCompareNames, JOrderedSetT::kSortAscending, kJFalse);
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
	const JCharacter* name
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

JBoolean
JNamedTreeNode::FindNamedChild
	(
	const JCharacter*	name,
	JNamedTreeNode**	node
	)
{
	const JNamedTreeNode* n;
	const JBoolean found = FindNamedChild(name, &n);
	if (found)
		{
		*node = const_cast<JNamedTreeNode*>(n);
		}
	return found;
}

JBoolean
JNamedTreeNode::FindNamedChild
	(
	const JCharacter*		name,
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
			return kJTrue;
			}
		}

	*node = NULL;
	return kJFalse;
}

/******************************************************************************
 GetNamedParent

 ******************************************************************************/

JNamedTreeNode*
JNamedTreeNode::GetNamedParent()
{
	JNamedTreeNode* node = dynamic_cast(JNamedTreeNode*, GetParent());
	assert (node != NULL);
	return node;
}

const JNamedTreeNode*
JNamedTreeNode::GetNamedParent()
	const
{
	const JNamedTreeNode* node = dynamic_cast(const JNamedTreeNode*, GetParent());
	assert (node != NULL);
	return node;
}

JBoolean
JNamedTreeNode::GetNamedParent
	(
	JNamedTreeNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast(JNamedTreeNode*, p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
		return kJFalse;
		}
}

JBoolean
JNamedTreeNode::GetNamedParent
	(
	const JNamedTreeNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast(const JNamedTreeNode*, p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
		return kJFalse;
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
	JNamedTreeNode* node = dynamic_cast(JNamedTreeNode*, GetChild(index));
	assert (node != NULL);
	return node;
}

const JNamedTreeNode*
JNamedTreeNode::GetNamedChild
	(
	const JIndex index
	)
	const
{
	const JNamedTreeNode* node = dynamic_cast(const JNamedTreeNode*, GetChild(index));
	assert (node != NULL);
	return node;
}

/******************************************************************************
 DynamicCastCompareNames (static protected)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::DynamicCastCompareNames
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	return CompareNames(dynamic_cast(JNamedTreeNode*, e1),
						dynamic_cast(JNamedTreeNode*, e2));
}

/******************************************************************************
 CompareNames (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::CompareNames
	(
	JNamedTreeNode * const & e1,
	JNamedTreeNode * const & e2
	)
{
	JOrderedSetT::CompareResult result =
		JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		result = JCompareStringsCaseSensitive(&(e1->itsName), &(e2->itsName));
		}
	return result;
}

/******************************************************************************
 DynamicCastCompareNamesForIncrSearch (static protected)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::DynamicCastCompareNamesForIncrSearch
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	return CompareNamesForIncrSearch(dynamic_cast(JNamedTreeNode*, e1),
									 dynamic_cast(JNamedTreeNode*, e2));
}

/******************************************************************************
 CompareNamesForIncrSearch (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::CompareNamesForIncrSearch
	(
	JNamedTreeNode * const & e1,
	JNamedTreeNode * const & e2
	)
{
	return JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
}
