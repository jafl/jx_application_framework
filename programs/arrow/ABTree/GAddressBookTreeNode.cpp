/******************************************************************************
 GAddressBookTreeNode.cc

	Data structure for JXFileTree.

	Copyright <A9> 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include "GAddressBookTreeNode.h"
#include "GAddressEntryTreeNode.h"
#include <JDirEntry.h>
#include <jGlobals.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

******************************************************************************/

GAddressBookTreeNode::GAddressBookTreeNode
	(
	JDirEntry*	entry,
	JNamedTreeNode* parent,
	const char*	name,
	const JBoolean	isBranch
	)
	:
	GAddressBaseTreeNode(parent, name, isBranch)
{
	itsEntry = entry;
}

/******************************************************************************
 Destructor

******************************************************************************/

GAddressBookTreeNode::~GAddressBookTreeNode()
{
	delete itsEntry;
}

/******************************************************************************
 OKToChange

 ******************************************************************************/

JBoolean
GAddressBookTreeNode::OKToChange()
	const
{
	return itsEntry->IsWritable();
}
/******************************************************************************
 GetFullPathAndName

 ******************************************************************************/

JString
GAddressBookTreeNode::GetFullPathAndName()
	const
{
	return itsEntry->GetFullName();
}

/******************************************************************************
 SetNameAndUpdate (public)

 ******************************************************************************/

void
GAddressBookTreeNode::SetNameAndUpdate
	(
	const JCharacter* newName
	)
{
	SetName(newName);
	JString path = itsEntry->GetPath();
	delete itsEntry;
	itsEntry = new JDirEntry(path, newName);
}

/******************************************************************************
 Save (public)

 ******************************************************************************/

void
GAddressBookTreeNode::Save()
{
	if (OKToChange())
		{
		ofstream os(GetFullPathAndName());
		Save(os);
		}
}

/******************************************************************************
 Save (public)

 ******************************************************************************/

void
GAddressBookTreeNode::Save
	(
	ostream& os
	)
{
	JSize count = GetChildCount();
	for (JSize i = 1; i <= count; i++)
		{
		JTreeNode* jchild = GetChild(i);
		GAddressEntryTreeNode* entry =
			dynamic_cast(GAddressEntryTreeNode*, jchild);
		assert(entry != NULL);
		entry->Save(os);
		}
}
