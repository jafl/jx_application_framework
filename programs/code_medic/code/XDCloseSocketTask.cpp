/******************************************************************************
 XDCloseSocketTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDCloseSocketTask.h"
#include "XDSocket.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

XDCloseSocketTask::XDCloseSocketTask
	(
	XDSocket* socket
	)
	:
	itsSocket(socket)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDCloseSocketTask::~XDCloseSocketTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
XDCloseSocketTask::Perform()
{
	itsSocket->close();
}
