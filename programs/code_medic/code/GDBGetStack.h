/******************************************************************************
 GDBGetStack.h

	Copyright (C) 2001 by John Lindal.

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

	virtual void	HandleSuccess(const JString& data) override;

private:

	GDBGetStackArguments*	itsArgsCmd;

private:

	// not allowed

	GDBGetStack(const GDBGetStack& source);
	const GDBGetStack& operator=(const GDBGetStack& source);
};

#endif
