/******************************************************************************
 JVMGetStack.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetStack
#define _H_JVMGetStack

#include "CMGetStack.h"

class JVMGetStack : public CMGetStack
{
public:

	JVMGetStack(JTree* tree, CMStackWidget* widget);

	virtual	~JVMGetStack();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMGetStack(const JVMGetStack& source);
	const JVMGetStack& operator=(const JVMGetStack& source);
};

#endif
