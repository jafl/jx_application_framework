/******************************************************************************
 GAddressBookTreeNode.h

	Interface for GAddressBookTreeNode class.

	Copyright <A9> 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GAddressBookTreeNode
#define _H_GAddressBookTreeNode

#include "GAddressBaseTreeNode.h"

class JString;
class JDirEntry;

class GAddressBookTreeNode : public GAddressBaseTreeNode
{
public:

	GAddressBookTreeNode(JDirEntry*	entry,
						JNamedTreeNode* parent = NULL,
						const char* name = "BASE",
						const JBoolean isBranch = kJTrue);

	virtual ~GAddressBookTreeNode();

	virtual JBoolean OKToChange() const;
	virtual JString	 GetFullPathAndName() const;
	void			 SetNameAndUpdate(const JCharacter* newName);

	void			 Save();
	virtual void	 Save(std::ostream& os);

private:

	JDirEntry*	itsEntry;

private:

	// not allowed

	GAddressBookTreeNode(const GAddressBookTreeNode& source);
	const GAddressBookTreeNode& operator=(const GAddressBookTreeNode& source);

};
#endif
