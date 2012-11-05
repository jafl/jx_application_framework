/******************************************************************************
 CMThreadNode.h

	Copyright © 2011 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMThreadNode
#define _H_CMThreadNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNamedTreeNode.h>

class CMThreadNode : public JNamedTreeNode
{
public:

	CMThreadNode(const JUInt64 id, const JCharacter* name,
				 const JCharacter* fileName, const JIndex lineIndex);

	virtual	~CMThreadNode();

	JUInt64		GetID() const;
	JBoolean	GetFile(JString* fileName, JIndex* lineIndex) const;

	void	AppendThread(JTreeNode* node);

private:

	JUInt64	itsID;
	JString	itsFileName;	// relative path
	JIndex	itsLineIndex;

private:

	// not allowed

	CMThreadNode(const CMThreadNode& source);
	const CMThreadNode& operator=(const CMThreadNode& source);
};


/******************************************************************************
 GetID

 ******************************************************************************/

inline JUInt64
CMThreadNode::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetFile

	This returns the relative path provided by gdb.

 ******************************************************************************/

inline JBoolean
CMThreadNode::GetFile
	(
	JString*	fileName,
	JIndex*		lineIndex
	)
	const
{
	*fileName  = itsFileName;
	*lineIndex = itsLineIndex;
	return JI2B(!itsFileName.IsEmpty() && itsLineIndex > 0);
}

/******************************************************************************
 AppendThread

 *****************************************************************************/

inline void
CMThreadNode::AppendThread
	(
	JTreeNode* node
	)
{
	Append(node);
	ShouldBeOpenable(kJTrue);
}

#endif
