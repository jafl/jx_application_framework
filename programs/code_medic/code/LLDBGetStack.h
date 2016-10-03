/******************************************************************************
 LLDBGetStack.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetStack
#define _H_LLDBGetStack

#include "CMGetStack.h"

class LLDBGetStackArguments;

class LLDBGetStack : public CMGetStack
{
public:

	LLDBGetStack(JTree* tree, CMStackWidget* widget);

	virtual	~LLDBGetStack();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetStack(const LLDBGetStack& source);
	const LLDBGetStack& operator=(const LLDBGetStack& source);
};

#endif
