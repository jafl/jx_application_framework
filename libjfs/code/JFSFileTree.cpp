/******************************************************************************
 JFSFileTree.cpp

	BASE CLASS = JTree

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "JFSFileTree.h"
#include "JFSFileTreeNodeBase.h"
#include <jAssert.h>

const JUtf8Byte* JFSFileTree::kDirectoryRenamed = "DirectoryRenamed::JFSFileTree";

/******************************************************************************
 Constructor

 ******************************************************************************/

JFSFileTree::JFSFileTree
	(
	JFSFileTreeNodeBase* root
	)
	:
	JTree(root)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFSFileTree::~JFSFileTree()
{
}

/******************************************************************************
 BroadcastDirectoryRenamed

 ******************************************************************************/

void
JFSFileTree::BroadcastDirectoryRenamed
	(
	const JString& oldPath,
	const JString& newPath
	)
{
	Broadcast(DirectoryRenamed(oldPath, newPath));
}

/******************************************************************************
 GetFSRoot

 ******************************************************************************/

JFSFileTreeNodeBase*
JFSFileTree::GetFSRoot()
{
	JFSFileTreeNodeBase* root = dynamic_cast<JFSFileTreeNodeBase*>(GetRoot());
	assert( root != nullptr );
	return root;
}

const JFSFileTreeNodeBase*
JFSFileTree::GetFSRoot()
	const
{
	const JFSFileTreeNodeBase* root = dynamic_cast<const JFSFileTreeNodeBase*>(GetRoot());
	assert( root != nullptr );
	return root;
}
