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
	const bool isOpenable
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

bool
JFSFileTreeNodeBase::Update
	(
	const bool			force,
	JFSFileTreeNodeBase**	updateNode
	)
{
	bool changed = false;

	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* node = dynamic_cast<JFSFileTreeNodeBase*>(GetChild(i));
		assert (node != nullptr);
		if (node->Update(force, updateNode))
			{
			changed = true;
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
	auto* tree = dynamic_cast<JFSFileTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const JFSFileTree*
JFSFileTreeNodeBase::GetFSFileTree()
	const
{
	const auto* tree = dynamic_cast<const JFSFileTree*>(GetTree());
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
	auto* n = dynamic_cast<JFSFileTreeNodeBase*>(p);
	assert( n != nullptr );
	return n;
}

const JFSFileTreeNodeBase*
JFSFileTreeNodeBase::GetFSParent()
	const
{
	const JTreeNode* p           = GetParent();
	const auto* n = dynamic_cast<const JFSFileTreeNodeBase*>(p);
	assert( n != nullptr );
	return n;
}

bool
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
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

bool
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
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}
