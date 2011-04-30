/******************************************************************************
 JFSFileTree.cpp

	BASE CLASS = JTree

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include "JFSFileTree.h"
#include "JFSFileTreeNodeBase.h"
#include <jAssert.h>

const JCharacter* JFSFileTree::kDirectoryRenamed = "DirectoryRenamed::JFSFileTree";

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
	assert( root != NULL );
	return root;
}

const JFSFileTreeNodeBase*
JFSFileTree::GetFSRoot()
	const
{
	const JFSFileTreeNodeBase* root = dynamic_cast<const JFSFileTreeNodeBase*>(GetRoot());
	assert( root != NULL );
	return root;
}
