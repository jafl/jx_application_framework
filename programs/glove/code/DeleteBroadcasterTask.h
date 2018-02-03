/******************************************************************************
 DeleteBroadcasterTask.h

	Interface for the DeleteBroadcasterTask class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_DeleteBroadcasterTask
#define _H_DeleteBroadcasterTask

#include <JXUrgentTask.h>

class JBroadcaster;

class DeleteBroadcasterTask : public JXUrgentTask
{
public:

	DeleteBroadcasterTask(JBroadcaster* object);

	virtual ~DeleteBroadcasterTask();

	virtual void	Perform();

private:

	JBroadcaster* 		itsObject;

	// not allowed

	DeleteBroadcasterTask(const DeleteBroadcasterTask& source);
	const DeleteBroadcasterTask& operator=(const DeleteBroadcasterTask& source);
};

#endif
