/******************************************************************************
 JFSFileTreeNodeBase.cpp

	Base class for parents of JFSFileTreeNodes.

	BASE CLASS = JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "JFSFileTreeNodeBase.h"
#include "JFSFileTree.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFSFileTreeNodeBase::JFSFileTreeNodeBase
	(
	const JBoolean isOpenable
	)
	:
	JNamedTreeNode(nullptr, JString::empty, isOpenable)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFSFileTreeNodeBase::~JFSFileTreeNodeBase()
{
}

/******************************************************************************
 Update (virtual)

	Convenience implementation that calls Update() on children.

 ******************************************************************************/

JBoolean
JFSFileTreeNodeBase::Update
	(
	const JBoolean			force,
	JFSFileTreeNodeBase**	updateNode
	)
{
	JBoolean changed = kJFalse;

	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		JFSFileTreeNodeBase* node = dynamic_cast<JFSFileTreeNodeBase*>(GetChild(i));
		assert (node != nullptr);
		if (node->Update(force, updateNode))
			{
			changed = kJTrue;
			}
		}

	return changed;
}

/******************************************************************************
 GetFSFileTree

 ******************************************************************************/

JFSFileTree*
JFSFileTreeNodeBase::GetFSFileTree()
{
	JFSFileTree* tree = dynamic_cast<JFSFileTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const JFSFileTree*
JFSFileTreeNodeBase::GetFSFileTree()
	const
{
	const JFSFileTree* tree = dynamic_cast<const JFSFileTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

/******************************************************************************
 GetFSParent

 ******************************************************************************/

JFSFileTreeNodeBase*
JFSFileTreeNodeBase::GetFSParent()
{
	JTreeNode* p           = GetParent();
	JFSFileTreeNodeBase* n = dynamic_cast<JFSFileTreeNodeBase*>(p);
	assert( n != nullptr );
	return n;
}

const JFSFileTreeNodeBase*
JFSFileTreeNodeBase::GetFSParent()
	const
{
	const JTreeNode* p           = GetParent();
	const JFSFileTreeNodeBase* n = dynamic_cast<const JFSFileTreeNodeBase*>(p);
	assert( n != nullptr );
	return n;
}

JBoolean
JFSFileTreeNodeBase::GetFSParent
	(
	JFSFileTreeNodeBase** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<JFSFileTreeNodeBase*>(p);
		assert( *parent != nullptr );
		return kJTrue;
		}
	else
		{
		*parent = nullptr;
		return kJFalse;
		}
}

JBoolean
JFSFileTreeNodeBase::GetFSParent
	(
	const JFSFileTreeNodeBase** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<const JFSFileTreeNodeBase*>(p);
		assert( *parent != nullptr );
		return kJTrue;
		}
	else
		{
		*parent = nullptr;
		return kJFalse;
		}
}
