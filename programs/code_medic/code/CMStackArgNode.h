/******************************************************************************
 CMStackArgNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMStackArgNode
#define _H_CMStackArgNode

#include <JNamedTreeNode.h>

class CMStackFrameNode;

class CMStackArgNode : public JNamedTreeNode
{
public:

	CMStackArgNode(CMStackFrameNode* parent,
				   const JString& name, const JString& value);

	virtual	~CMStackArgNode();

	const JString&	GetValue() const;

private:

	JString	itsValue;

private:

	// not allowed

	CMStackArgNode(const CMStackArgNode& source);
	const CMStackArgNode& operator=(const CMStackArgNode& source);
};


/******************************************************************************
 GetValue

 ******************************************************************************/

inline const JString&
CMStackArgNode::GetValue()
	const
{
	return itsValue;
}

#endif
