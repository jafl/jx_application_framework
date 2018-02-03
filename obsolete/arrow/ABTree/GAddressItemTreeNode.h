/******************************************************************************
 GAddressItemTreeNode.h

	Interface for GAddressItemTreeNode class.

	Copyright <A9> 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GAddressItemTreeNode
#define _H_GAddressItemTreeNode

#include "GAddressBaseTreeNode.h"
#include <JString.h>

class GAddressItemTreeNode : public GAddressBaseTreeNode
{
public:

	enum Type
	{
		kName = 1,
		kFcc,
		kComment,
		kEMail
	};

public:

	GAddressItemTreeNode(Type type,
						JNamedTreeNode* parent = NULL,
						const char* name = "BASE",
						const JBoolean isBranch = kJTrue);

	virtual ~GAddressItemTreeNode();

	virtual JString	GetFullPathAndName() const;

	Type			GetType() const;
	const JString&	GetText() const;
	void			SetText(const JString& text);

	virtual JBoolean OKToChange() const;
	virtual void	 Save(std::ostream& os);

private:

	Type	itsType;
	JString itsText;

private:

	// not allowed

	GAddressItemTreeNode(const GAddressItemTreeNode& source);
	const GAddressItemTreeNode& operator=(const GAddressItemTreeNode& source);

};

/******************************************************************************
 GetType (public)

 ******************************************************************************/

inline GAddressItemTreeNode::Type
GAddressItemTreeNode::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 GetText (public)

 ******************************************************************************/

inline const JString&
GAddressItemTreeNode::GetText()
	const
{
	return itsText;
}

#endif
