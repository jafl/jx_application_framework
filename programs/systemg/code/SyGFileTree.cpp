/******************************************************************************
 SyGFileTree.cpp

	BASE CLASS = JFSFileTree

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGFileTree.h"
#include "SyGFileTreeNode.h"
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFileTree::SyGFileTree
	(
	SyGFileTreeNode* root
	)
	:
	JFSFileTree(root)
{
	root->Init();	// build child list
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFileTree::~SyGFileTree()
{
}

/******************************************************************************
 GetDirectory

 ******************************************************************************/

const JString&
SyGFileTree::GetDirectory()
	const
{
	return GetRootDirInfo()->GetDirectory();
}

/******************************************************************************
 Update

	Returns kJTrue if anything changed.

 ******************************************************************************/

JBoolean
SyGFileTree::Update
	(
	const JBoolean			force,
	JFSFileTreeNodeBase**	updateNode
	)
{
	if (force || updateNode == NULL || *updateNode == NULL)
		{
		return GetSyGRoot()->Update(force, updateNode);
		}
	else
		{
		return (**updateNode).Update(force, updateNode);
		}
}

/******************************************************************************
 HiddenVisible

 ******************************************************************************/

JBoolean
SyGFileTree::HiddenVisible()
	const
{
	return GetRootDirInfo()->HiddenVisible();
}

/******************************************************************************
 ShowHidden

 ******************************************************************************/

void
SyGFileTree::ShowHidden
	(
	const JBoolean show
	)
{
	GetRootDirInfo()->ShowHidden(show);
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
SyGFileTree::SetWildcardFilter
	(
	const JCharacter* filter
	)
{
	GetRootDirInfo()->SetWildcardFilter(filter);
}

/******************************************************************************
 ClearWildcardFilter

 ******************************************************************************/

void
SyGFileTree::ClearWildcardFilter()
{
	GetRootDirInfo()->ClearWildcardFilter();
}

/******************************************************************************
 SetNodeCompareFunction

 ******************************************************************************/

void
SyGFileTree::SetNodeCompareFunction
	(
	const GFMColType type
	)
{
	GetSyGRoot()->SetChildCompareFunction(type);
}

/******************************************************************************
 GetSyGRoot

 ******************************************************************************/

SyGFileTreeNode*
SyGFileTree::GetSyGRoot()
{
	SyGFileTreeNode* root = dynamic_cast<SyGFileTreeNode*>(GetRoot());
	assert( root != NULL );
	return root;
}

const SyGFileTreeNode*
SyGFileTree::GetSyGRoot()
	const
{
	const SyGFileTreeNode* root = dynamic_cast<const SyGFileTreeNode*>(GetRoot());
	assert( root != NULL );
	return root;
}

/******************************************************************************
 GetRootDirInfo

 ******************************************************************************/

JDirInfo*
SyGFileTree::GetRootDirInfo()
{
	JDirInfo* info;
	const JBoolean ok = GetSyGRoot()->GetDirInfo(&info);
	assert( ok );
	return info;
}

const JDirInfo*
SyGFileTree::GetRootDirInfo()
	const
{
	const JDirInfo* info;
	const JBoolean ok = GetSyGRoot()->GetDirInfo(&info);
	assert( ok );
	return info;
}
