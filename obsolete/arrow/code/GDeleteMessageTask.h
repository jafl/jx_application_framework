/******************************************************************************
 GDeleteMessageTask.h

	Interface for the GDeleteMessageTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDeleteMessageTask
#define _H_GDeleteMessageTask

#include <JXUrgentTask.h>

class SMTPMessage;

class GDeleteMessageTask : public JXUrgentTask
{
public:

	GDeleteMessageTask(SMTPMessage* message);

	virtual ~GDeleteMessageTask();

	virtual void	Perform();

private:

	SMTPMessage*	itsMessage;

private:

	// not allowed

	GDeleteMessageTask(const GDeleteMessageTask& source);
	const GDeleteMessageTask& operator=(const GDeleteMessageTask& source);
};

#endif
