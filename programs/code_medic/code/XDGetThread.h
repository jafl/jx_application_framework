/******************************************************************************
 XDGetThread.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThread
#define _H_XDGetThread

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
