/******************************************************************************
 XDGetThread.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetThread
#define _H_XDGetThread

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThread.h"

class XDGetThread : public CMGetThread
{
public:

	XDGetThread(CMThreadsWidget* widget);

	virtual	~XDGetThread();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDGetThread(const XDGetThread& source);
	const XDGetThread& operator=(const XDGetThread& source);
};

#endif
