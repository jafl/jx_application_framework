/******************************************************************************
 JVMGetThreads.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetThreads
#define _H_JVMGetThreads

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThreads.h"

class JTree;
class JTreeNode;
class JVMThreadNode;

class JVMGetThreads : public CMGetThreads
{
public:

	JVMGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~JVMGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	JTree*	itsTree;

private:

	void	CopyTree(JVMThreadNode* src, JTreeNode* dest);

	// not allowed

	JVMGetThreads(const JVMGetThreads& source);
	const JVMGetThreads& operator=(const JVMGetThreads& source);
};

#endif
