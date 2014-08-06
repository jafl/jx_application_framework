/******************************************************************************
 GMailFileTreeNode.cc

	Data structure for JXFileTree.

	Copyright <A9> 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GMailFileTreeNode.h"
#include "GMGlobals.h"
#include "GMFileTree.h"
#include "GMailboxUtils.h"

#include <JTreeList.h>
#include <JColormap.h>
#include <JPtrArray-JString.h>
#include <JDirInfo.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <sys/stat.h>
#include <unistd.h>
#include <jAssert.h>


/******************************************************************************
	Constructor

******************************************************************************/

GMailFileTreeNode::GMailFileTreeNode
	(
	JDirEntry*	entry
	)
	:
	JFSFileTreeNode(entry)
{
	GMailFileTreeNodeX();
}

void
GMailFileTreeNode::GMailFileTreeNodeX()
{
	itsHasNewMail	= kJFalse;
}


/******************************************************************************
	Destructor

******************************************************************************/

GMailFileTreeNode::~GMailFileTreeNode()
{
}

/******************************************************************************
 CreateChild (virtual protected)

 ******************************************************************************/

JFSFileTreeNode*
GMailFileTreeNode::CreateChild
	(
	JDirEntry* entry
	)
{
	GMailFileTreeNode* node	= new GMailFileTreeNode(entry);
	assert(node != NULL);
	return node;
}

/******************************************************************************
 UpdateNode (virtual public)

 ******************************************************************************/

JBoolean
GMailFileTreeNode::Update
	(
	const JBoolean force
	)
{
	JBoolean retValue			= JFSFileTreeNode::Update(force);

	if (GetDirEntry()->IsDirectory())
		{
		return retValue;
		}

	if (GetDirEntry()->GetModTime() > GetDirEntry()->GetAccessTime())
		{
		if (!itsHasNewMail)
			{
			JTree* jtree		= GetTree();
			GMFileTree* tree	= dynamic_cast<GMFileTree*>(jtree);
			assert(tree != NULL);
			tree->BroadcastNodeHasNewMail(this, kJTrue);
			itsHasNewMail		= kJTrue;
			}
		}
	else if (itsHasNewMail)
		{
		JTree* jtree			= GetTree();
		GMFileTree* tree		= dynamic_cast<GMFileTree*>(jtree);
		assert(tree != NULL);
		tree->BroadcastNodeHasNewMail(this, kJFalse);
		itsHasNewMail			= kJFalse;
		}

	return retValue;
}
