/******************************************************************************
 JVMGetStack.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetStack
#define _H_JVMGetStack

#include "CMGetStack.h"

class JVMGetStack : public CMGetStack
{
public:

	JVMGetStack(JTree* tree, CMStackWidget* widget);

	virtual	~JVMGetStack();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMGetStack(const JVMGetStack& source);
	const JVMGetStack& operator=(const JVMGetStack& source);
};

#endif
