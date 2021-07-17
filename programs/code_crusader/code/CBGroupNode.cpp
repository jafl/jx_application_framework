/******************************************************************************
 CBGroupNode.cpp

	BASE CLASS = CBProjectNode

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "CBGroupNode.h"
#include "CBProjectTree.h"
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The second version requires the parent to be specified because
	the tree hasn't been fully constructed yet.

 ******************************************************************************/

CBGroupNode::CBGroupNode
	(
	CBProjectTree*	tree,
	const bool	append
	)
	:
	CBProjectNode(tree, kCBGroupNT, JGetString("NewGroupName::CBGroupNode"), true)
{
	if (tree != nullptr && append)
		{
		(tree->GetRoot())->Append(this);
		}
}

CBGroupNode::CBGroupNode
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBProjectNode*		parent
	)
	:
	CBProjectNode(input, vers, parent, kCBGroupNT, true)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGroupNode::~CBGroupNode()
{
}

/******************************************************************************
 Print (virtual)

 ******************************************************************************/

void
CBGroupNode::Print
	(
	JString* text
	)
	const
{
	*text += "\n";
	*text += GetName();
	*text += "\n\n";

	CBProjectNode::Print(text);
}

/******************************************************************************
 ShowFileLocation (virtual)

 ******************************************************************************/

void
CBGroupNode::ShowFileLocation()
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		(GetProjectChild(i))->ShowFileLocation();
		}
}

/******************************************************************************
 ViewPlainDiffs (virtual)

 ******************************************************************************/

void
CBGroupNode::ViewPlainDiffs
	(
	const bool origSilent
	)
	const
{
	const JSize count     = GetChildCount();
	const bool silent = origSilent || count > 1;
	for (JIndex i=1; i<=count; i++)
		{
		(GetProjectChild(i))->ViewPlainDiffs(silent);
		}
}

/******************************************************************************
 ViewVCSDiffs (virtual)

 ******************************************************************************/

void
CBGroupNode::ViewVCSDiffs
	(
	const bool origSilent
	)
	const
{
	const JSize count     = GetChildCount();
	const bool silent = origSilent || count > 1;
	for (JIndex i=1; i<=count; i++)
		{
		(GetProjectChild(i))->ViewVCSDiffs(silent);
		}
}
