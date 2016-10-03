/******************************************************************************
 JVMThreadNode.h

	Copyright (C) 2011 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JVMThreadNode
#define _H_JVMThreadNode

#include "CMThreadNode.h"

class JVMThreadNode : public CMThreadNode
{
public:

	enum
	{
		kRootThreadGroupID = 0	// 0 is reserved for null object
	};

	enum Type
	{
		kThreadType,
		kGroupType
	};

public:

	JVMThreadNode(const Type type, const JUInt64 id);
	JVMThreadNode(const JUInt64 id);
	JVMThreadNode(const JVMThreadNode& source);

	virtual	~JVMThreadNode();

	Type	GetType() const;
	void	FindParent(const JUInt64 id);

	static JOrderedSetT::CompareResult
		CompareID(JVMThreadNode* const & t1, JVMThreadNode* const & t2);

protected:

	virtual void	NameChanged();

private:

	const Type	itsType;

private:

	// not allowed

	const JVMThreadNode& operator=(const JVMThreadNode& source);
};


/******************************************************************************
 GetType

 ******************************************************************************/

inline JVMThreadNode::Type
JVMThreadNode::GetType()
	const
{
	return itsType;
}

#endif
