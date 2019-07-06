/******************************************************************************
 XDGetStack.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetStack
#define _H_XDGetStack

#include "CMGetStack.h"

class XDGetStack : public CMGetStack
{
public:

	XDGetStack(JTree* tree, CMStackWidget* widget);

	virtual	~XDGetStack();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	XDGetStack(const XDGetStack& source);
	const XDGetStack& operator=(const XDGetStack& source);
};

#endif
