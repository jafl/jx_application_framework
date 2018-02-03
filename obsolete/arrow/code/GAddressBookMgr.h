/******************************************************************************
 GAddressBookMgr.h

	Interface for the GAddressBookMgr class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GAddressBookMgr
#define _H_GAddressBookMgr

#include <JBroadcaster.h>
#include <JString.h>
#include <JStringPtrMap.h>

#include <GAddressBookEntry.h>

#include <jTypes.h>

class JTree;
class GAddressEntryTreeNode;

class GAddressBookMgr : virtual public JBroadcaster
{
public:

	GAddressBookMgr();
	virtual ~GAddressBookMgr();

	JBoolean	NameIsAlias(const JCharacter* name, JString& alias, JString& fcc);

	void		AddDefaultAddressBook(JTree* tree);
	void		AddAddressBook(const JCharacter* name, JTree* tree);

	JBoolean	ChangeNickName(const JCharacter* oldname, const JCharacter* newname, JTree* tree);
	void		AdjustProperties(GAddressEntryTreeNode* node);

	JBoolean	NewNickName(const JCharacter* addresses, JString* name);
	void		DeleteName(const JCharacter* name);

protected:


private:

	JStringPtrMap<GAddressBookEntry>*	itsAddresses;

private:

	JBoolean	GetNextRecord(JString& line, JString& record, std::istream& is);

	// not allowed

	GAddressBookMgr(const GAddressBookMgr& source);
	const GAddressBookMgr& operator=(const GAddressBookMgr& source);
};

#endif
