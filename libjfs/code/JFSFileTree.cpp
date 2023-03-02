/******************************************************************************
 JFSFileTree.cpp

	BASE CLASS = JTree

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "JFSFileTree.h"
#include "JFSFileTreeNodeBase.h"
#include <jx-af/jcore/jAssert.h>

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
	auto* root = dynamic_cast<JFSFileTreeNodeBase*>(GetRoot());
	assert( root != nullptr );
	return root;
}

const JFSFileTreeNodeBase*
JFSFileTree::GetFSRoot()
	const
{
	const auto* root = dynamic_cast<const JFSFileTreeNodeBase*>(GetRoot());
	assert( root != nullptr );
	return root;
}
