/******************************************************************************
 GMFileTree.cc

	File Tree

	BASE CLASS = JTree

 ******************************************************************************/

#include "GMFileTree.h"
#include "JTreeList.h"

#include "jAssert.h"

const JCharacter* GMFileTree::kNodeHasNewMail	= "kNodeHasNewMail::GMFileTree";

/******************************************************************************
 Constructor

 ******************************************************************************/

GMFileTree::GMFileTree
	(
	JTreeNode* root
	)
	:
	JTree(root)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMFileTree::~GMFileTree()
{
}

/******************************************************************************
 BroadcastNodeHasNewMail (public)

 ******************************************************************************/

void
GMFileTree::BroadcastNodeHasNewMail
	(
	JTreeNode*		node,
	const JBoolean	newmail
	)
{
	Broadcast(NodeHasNewMail(node, newmail));
}
