/******************************************************************************
 XDCloseSocketTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDCloseSocketTask
#define _H_XDCloseSocketTask

#include <JXUrgentTask.h>

class XDSocket;

class XDCloseSocketTask : public JXUrgentTask
{
public:

	XDCloseSocketTask(XDSocket* socket);

	virtual ~XDCloseSocketTask();

	virtual void	Perform() override;

private:

	XDSocket*	itsSocket;

private:

	// not allowed

	XDCloseSocketTask(const XDCloseSocketTask& source);
	const XDCloseSocketTask& operator=(const XDCloseSocketTask& source);
};

#endif
