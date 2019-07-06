/******************************************************************************
 XDGetThreads.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThreads
#define _H_XDGetThreads

#include "CMGetThreads.h"

class JTree;

class XDGetThreads : public CMGetThreads
{
public:

	XDGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~XDGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	XDGetThreads(const XDGetThreads& source);
	const XDGetThreads& operator=(const XDGetThreads& source);
};

#endif
