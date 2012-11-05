/******************************************************************************
 XDCloseSocketTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDCloseSocketTask
#define _H_XDCloseSocketTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class XDSocket;

class XDCloseSocketTask : public JXUrgentTask
{
public:

	XDCloseSocketTask(XDSocket* socket);

	virtual ~XDCloseSocketTask();

	virtual void	Perform();

private:

	XDSocket*	itsSocket;

private:

	// not allowed

	XDCloseSocketTask(const XDCloseSocketTask& source);
	const XDCloseSocketTask& operator=(const XDCloseSocketTask& source);
};

#endif
