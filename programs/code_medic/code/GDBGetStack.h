/******************************************************************************
 GDBGetStack.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetStack
#define _H_GDBGetStack

#include "CMGetStack.h"

class GDBGetStackArguments;

class GDBGetStack : public CMGetStack
{
public:

	GDBGetStack(JTree* tree, CMStackWidget* widget);

	virtual	~GDBGetStack();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	GDBGetStackArguments*	itsArgsCmd;

private:

	// not allowed

	GDBGetStack(const GDBGetStack& source);
	const GDBGetStack& operator=(const GDBGetStack& source);
};

#endif
