/******************************************************************************
 GAddressEntryTreeNode.h

	Interface for GAddressEntryTreeNode class.

	Copyright <A9> 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GAddressEntryTreeNode
#define _H_GAddressEntryTreeNode

#include "GAddressBaseTreeNode.h"

class JString;

class GAddressEntryTreeNode : public GAddressBaseTreeNode
{
public:

	GAddressEntryTreeNode(JNamedTreeNode* parent = NULL,
						const char* name = "BASE",
						const JBoolean isBranch = kJTrue);

	virtual ~GAddressEntryTreeNode();
	virtual JString	 GetFullPathAndName() const;
	virtual JBoolean OKToChange() const;

	JBoolean	HasFcc() const;
	JBoolean	HasComment() const;

	void		SetHasFcc(const JBoolean fcc);
	void		SetHasComment(const JBoolean comment);

	virtual void	Save(std::ostream& os);
	JString			GetFullAddress(const JBoolean withRet = kJFalse);

private:

	JBoolean itsHasFcc;
	JBoolean itsHasComment;
	JBoolean itsIsDestructing;

private:

	// not allowed

	GAddressEntryTreeNode(const GAddressEntryTreeNode& source);
	const GAddressEntryTreeNode& operator=(const GAddressEntryTreeNode& source);

};

/******************************************************************************
 HasFcc (public)

 ******************************************************************************/

inline JBoolean
GAddressEntryTreeNode::HasFcc()
	const
{
	return itsHasFcc;
}

/******************************************************************************
 HasComment (public)

 ******************************************************************************/

inline JBoolean
GAddressEntryTreeNode::HasComment()
	const
{
	return itsHasComment;
}

#endif
