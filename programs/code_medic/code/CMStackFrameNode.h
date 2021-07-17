/******************************************************************************
 CMStackFrameNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMStackFrameNode
#define _H_CMStackFrameNode

#include <JNamedTreeNode.h>

class CMStackFrameNode : public JNamedTreeNode
{
public:

	CMStackFrameNode(JTreeNode* parent,
					  const JUInt64 id, const JString& name,
					  const JString& fileName, const JIndex lineIndex);

	virtual	~CMStackFrameNode();

	JUInt64		GetID() const;
	bool	GetFile(JString* fileName, JIndex* lineIndex) const;

	void	AppendArg(JTreeNode* node);

private:

	JUInt64	itsID;
	JString	itsFileName;	// relative path
	JIndex	itsLineIndex;

private:

	// not allowed

	CMStackFrameNode(const CMStackFrameNode& source);
	const CMStackFrameNode& operator=(const CMStackFrameNode& source);
};


/******************************************************************************
 GetID

 ******************************************************************************/

inline JUInt64
CMStackFrameNode::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetFile

	This returns the relative path provided by gdb.

 ******************************************************************************/

inline bool
CMStackFrameNode::GetFile
	(
	JString*	fileName,
	JIndex*		lineIndex
	)
	const
{
	*fileName  = itsFileName;
	*lineIndex = itsLineIndex;
	return !itsFileName.IsEmpty() && itsLineIndex > 0;
}

/******************************************************************************
 AppendArg

 *****************************************************************************/

inline void
CMStackFrameNode::AppendArg
	(
	JTreeNode* node
	)
{
	Append(node);
	ShouldBeOpenable(true);
}

#endif
