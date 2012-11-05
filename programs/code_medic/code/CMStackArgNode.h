/******************************************************************************
 CMStackArgNode.h

	Copyright © 2001 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_CMStackArgNode
#define _H_CMStackArgNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNamedTreeNode.h>

class CMStackFrameNode;

class CMStackArgNode : public JNamedTreeNode
{
public:

	CMStackArgNode(CMStackFrameNode* parent,
				   const JCharacter* name, const JCharacter* value);

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
