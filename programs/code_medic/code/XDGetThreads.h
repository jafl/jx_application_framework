/******************************************************************************
 XDGetThreads.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetThreads
#define _H_XDGetThreads

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThreads.h"

class JTree;

class XDGetThreads : public CMGetThreads
{
public:

	XDGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~XDGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDGetThreads(const XDGetThreads& source);
	const XDGetThreads& operator=(const XDGetThreads& source);
};

#endif
