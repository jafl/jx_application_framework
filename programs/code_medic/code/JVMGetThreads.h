/******************************************************************************
 JVMGetThreads.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreads
#define _H_JVMGetThreads

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

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;

private:

	void	CopyTree(JVMThreadNode* src, JTreeNode* dest);

	// not allowed

	JVMGetThreads(const JVMGetThreads& source);
	const JVMGetThreads& operator=(const JVMGetThreads& source);
};

#endif
