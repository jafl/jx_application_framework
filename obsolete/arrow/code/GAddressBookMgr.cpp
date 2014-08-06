/******************************************************************************
 GAddressBookMgr.cc


	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GAddressBookMgr.h"
#include "GAddressBookEntry.h"
#include "GAddressBookTreeNode.h"
#include "GAddressEntryTreeNode.h"
#include "GAddressItemTreeNode.h"

#include <JNamedTreeNode.h>
#include <JTree.h>

#include <GMGlobals.h>
#include <gMailUtils.h>

#include <JDirEntry.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>

#include <X11/keysym.h>
#include <stdio.h>
#include <jAssert.h>

/*****************************************************************************
 Constructor

 *****************************************************************************/

GAddressBookMgr::GAddressBookMgr()
{
	itsAddresses = new JStringPtrMap<GAddressBookEntry>(JPtrArrayT::kDeleteAll);
	assert(itsAddresses != NULL);
}

/*****************************************************************************
 Destructor

 *****************************************************************************/

GAddressBookMgr::~GAddressBookMgr()
{
	delete itsAddresses;
}

/*****************************************************************************
 NameIsAlias

 *****************************************************************************/

JBoolean
GAddressBookMgr::NameIsAlias
	(
	const JCharacter*	name,
	JString&			alias,
	JString&			fcc
	)
{
	GAddressBookEntry* entry;
	if (!itsAddresses->GetElement(name, &entry))
		{
		return kJFalse;
		}

	alias = entry->address;
	if (alias.IsEmpty())
		{
		return kJFalse;
		}

	if (alias.GetFirstCharacter() == '(' &&
		alias.GetLastCharacter() == ')')
		{
		if (entry->address.GetLength() > 2)
			{
			alias = entry->address.GetSubstring(2, entry->address.GetLength() - 1);
			}			
		}

	fcc = entry->fcc;
	if (!fcc.IsEmpty() && 
		fcc.GetFirstCharacter() == '(' &&
		fcc.GetLastCharacter() == ')')
		{
		if (entry->fcc.GetLength() > 2)
			{
			fcc = entry->fcc.GetSubstring(2, entry->fcc.GetLength() - 1);
			}
		}

	return kJTrue;
}

/*****************************************************************************
 GetNextRecord

 *****************************************************************************/

JBoolean
GAddressBookMgr::GetNextRecord
	(
	JString& line,
	JString& record,
	istream& is
	)
{
	if (line.IsEmpty())
		{
		return kJFalse;
		}
	JIndex index;
	if (line.LocateSubstring("\t", &index))
		{
		if (index > 1)
			{
			record = line.GetSubstring(1, index - 1);
			line.RemoveSubstring(1, index);
			return kJTrue;
			}
		line.RemoveSubstring(1, 1);
		return kJFalse;
		}
	record = line;
	if (record.Contains("(") && !record.Contains(")"))
		{
		JString temp = JReadUntil(is, ')');
		record += temp + ")";
		line = JReadLine(is);
		if (!line.IsEmpty() && (line.GetFirstCharacter() == '\t'))
			{
			line.RemoveSubstring(1, 1);
			}
		}
	else
		{
		line.Clear();
		}
	return kJTrue;
}

/******************************************************************************
 AddDefaultAddressBook

 ******************************************************************************/

void
GAddressBookMgr::AddDefaultAddressBook
	(
	JTree* tree
	)
{
	JString fullName;
	if (JGetPrefsDirectory(&fullName))
		{
		fullName = JCombinePathAndName(fullName, ".addressbook");
		AddAddressBook(fullName, tree);
		}
}

/******************************************************************************
 AddAddressBook (public)

 ******************************************************************************/

void
GAddressBookMgr::AddAddressBook
	(
	const JCharacter*	name,
	JTree*				tree
	)
{
	if (!JFileExists(name))
		{
		return;
		}
	JDirEntry* dirEntry = new JDirEntry(name);
	assert(dirEntry != NULL);
	JTreeNode* jbase = tree->GetRoot();
	JNamedTreeNode* base = dynamic_cast<JNamedTreeNode*>(jbase);
	assert(base != NULL);
	GAddressBookTreeNode* book = new
		GAddressBookTreeNode(dirEntry, base, dirEntry->GetName());
	assert(book != NULL);
	ifstream is(name);
	while (is.good())
		{
		JString line = JReadLine(is);
		GAddressBookEntry* entry = new GAddressBookEntry();
		assert( entry != NULL );

		JString name;
		if (GetNextRecord(line, name, is))
			{
			GetNextRecord(line, entry->fullname, is);
			if (GetNextRecord(line, entry->address, is))
				{
				GetNextRecord(line, entry->fcc, is);
				GetNextRecord(line, entry->comment, is);
				itsAddresses->SetElement(name, entry, JPtrArrayT::kDelete);

				GAddressEntryTreeNode* aEntry =
					new GAddressEntryTreeNode(book, entry->fullname);
				assert(aEntry != NULL);
				GAddressItemTreeNode* item =
					new GAddressItemTreeNode(GAddressItemTreeNode::kName,
											 aEntry, name, kJFalse);
				assert(item != NULL);

				JString address = entry->address;
				if (address.BeginsWith("(") && address.GetLength() > 2)
					{
					address = address.GetSubstring(2, address.GetLength() - 1);
					JPtrArray<JString> list(JPtrArrayT::kForgetAll);
					GParseNameList(address, list);
					const JSize count = list.GetElementCount();
					for (JSize i = count; i >= 1; i--)
						{
						item =
							new GAddressItemTreeNode(GAddressItemTreeNode::kEMail,
													 aEntry, *(list.NthElement(i)), kJFalse);
						assert(item != NULL);
						}
					list.DeleteAll();
					}
				else
					{
					item =
						new GAddressItemTreeNode(GAddressItemTreeNode::kEMail,
												 aEntry, address, kJFalse);
					}

				if (!entry->comment.IsEmpty())
					{
					item =
						new GAddressItemTreeNode(GAddressItemTreeNode::kComment,
												 aEntry, entry->comment, kJFalse);
					assert(item != NULL);
					}

				if (!entry->fcc.IsEmpty())
					{
					item =
						new GAddressItemTreeNode(GAddressItemTreeNode::kFcc,
												 aEntry, entry->fcc, kJFalse);
					assert(item != NULL);
					}

				book->InsertSorted(aEntry);

				continue;
				}
			}
		delete entry;
		}
}

/******************************************************************************
 ChangeNickName (public)

 ******************************************************************************/

JBoolean
GAddressBookMgr::ChangeNickName
	(
	const JCharacter* 	oldname,
	const JCharacter* 	newname,
	JTree* 				tree
	)
{
	GAddressBookEntry* entry;
	if (itsAddresses->GetElement(newname, &entry) ||
		!itsAddresses->GetElement(oldname, &entry))		// must be last, so we can operate on result
		{
		return kJFalse;
		}

	itsAddresses->RemoveElement(oldname);
	itsAddresses->SetElement(newname, entry, JPtrArrayT::kDelete);
	JTreeNode* root	= tree->GetRoot();
	JSize bcount	= root->GetChildCount();
	for (JIndex i = 1; i <= bcount; i++)
		{
		JTreeNode* jbook	= root->GetChild(i);
		GAddressBaseTreeNode* book = 
			dynamic_cast<GAddressBaseTreeNode*>(jbook);
		assert(book != NULL);
		if (book->OKToChange())
			{
			JSize ecount	= book->GetChildCount();
			for (JIndex j = 1; j <= ecount; j++)
				{
				JTreeNode* jentry	= book->GetChild(j);
				JBoolean kNeedsAdjustment	= kJFalse;
				JSize icount		= jentry->GetChildCount();
				for (JIndex k = 1; k <= icount; k++)
					{
					JTreeNode* jitem	= jentry->GetChild(k);
					GAddressItemTreeNode* item	=
						dynamic_cast<GAddressItemTreeNode*>(jitem);
					assert(item != NULL);
					if (item->GetType() == GAddressItemTreeNode::kEMail &&
						item->GetText() == oldname)
						{
						item->SetText(newname);
						kNeedsAdjustment	= kJTrue;
						tree->BroadcastChange(item);
						}
					}
				if (kNeedsAdjustment)
					{
					GAddressEntryTreeNode* entry =
						dynamic_cast<GAddressEntryTreeNode*>(jentry);
					assert(entry != NULL);
					AdjustProperties(entry);
					}
				}
			}
		}

	return kJTrue;
}

/******************************************************************************
 AdjustProperties (public)

 ******************************************************************************/

void
GAddressBookMgr::AdjustProperties
	(
	GAddressEntryTreeNode* node
	)
{
	JTreeNode* jchild1 = node->GetChild(1);
	GAddressItemTreeNode* child1 =
		dynamic_cast<GAddressItemTreeNode*>(jchild1);
	assert(child1 != NULL);
	GAddressBookEntry* entry;
	if (itsAddresses->GetElement(child1->GetText(), &entry))
		{
		entry->fullname = node->GetName();
		entry->fcc.Clear();
		entry->comment.Clear();
		const JSize count = node->GetChildCount();
		JSize expCount = 2;
		if (node->HasFcc())
			{
			expCount++;
			}
		if (node->HasComment())
			{
			expCount++;
			}
		JBoolean multi_address = kJFalse;
		if (expCount < count)
			{
			multi_address = kJTrue;
			}
		JString address;
		if (multi_address)
			{
			address = "(";
			}
		for (JSize i = 2; i <= count; i++)
			{
			JTreeNode* jchild = node->GetChild(i);
			GAddressItemTreeNode* child =
				dynamic_cast<GAddressItemTreeNode*>(jchild);
			assert(child != NULL);
			GAddressItemTreeNode::Type type = child->GetType();
			if (type == GAddressItemTreeNode::kFcc)
				{
				entry->fcc = child->GetText();
				}
			else if (type == GAddressItemTreeNode::kComment)
				{
				entry->comment = child->GetText();
				}
			else
				{
				if (!multi_address)
					{
					address = child->GetText();
					}
				else
					{
					if (address != "(")
						{
						address += ",";
						}
					address += child->GetText();
					if (i == count)
						{
						address += ")";
						}
					}
				}
			}
		entry->address = address;
		}
}

/******************************************************************************
 NewNickName (public)

 ******************************************************************************/

JBoolean
GAddressBookMgr::NewNickName
	(
	const JCharacter* addresses,
	JString*		  name
	)
{
	JString alias = "Untitled";
	JIndex count  = 1;
	GAddressBookEntry* entry;
	while (itsAddresses->GetElement(alias, &entry))
		{
		count++;
		if (count >= 100)
			{
			return kJFalse;
			}
		alias = "Untitled" + JString(count);
		}
	entry = new GAddressBookEntry();
	assert(entry != NULL);
	JString add(addresses);
	if (add.IsEmpty())
		{
		add = "nobody";
		}
	entry->address = add;
	itsAddresses->SetElement(alias, entry, JPtrArrayT::kDelete);
	*name = alias;
	return kJTrue;
}

/******************************************************************************
 DeleteName (public)

 ******************************************************************************/

void
GAddressBookMgr::DeleteName
	(
	const JCharacter* name
	)
{
	itsAddresses->DeleteElement(name);
}
