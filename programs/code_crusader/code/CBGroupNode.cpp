/******************************************************************************
 CBGroupNode.cpp

	BASE CLASS = CBProjectNode

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBGroupNode.h"
#include "CBProjectTree.h"
#include <jAssert.h>

static const JCharacter* kNewGroupName = "New group";

/******************************************************************************
 Constructor

	The second version requires the parent to be specified because
	the tree hasn't been fully constructed yet.

 ******************************************************************************/

CBGroupNode::CBGroupNode
	(
	CBProjectTree*	tree,
	const JBoolean	append
	)
	:
	CBProjectNode(tree, kCBGroupNT, kNewGroupName, kJTrue)
{
	if (tree != NULL && append)
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
	CBProjectNode(input, vers, parent, kCBGroupNT, kJTrue)
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
	const JBoolean origSilent
	)
	const
{
	const JSize count     = GetChildCount();
	const JBoolean silent = JI2B(origSilent || count > 1);
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
	const JBoolean origSilent
	)
	const
{
	const JSize count     = GetChildCount();
	const JBoolean silent = JI2B(origSilent || count > 1);
	for (JIndex i=1; i<=count; i++)
		{
		(GetProjectChild(i))->ViewVCSDiffs(silent);
		}
}
