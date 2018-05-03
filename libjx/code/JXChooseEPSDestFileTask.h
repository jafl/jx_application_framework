/******************************************************************************
 JXChooseEPSDestFileTask.h

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseEPSDestFileTask
#define _H_JXChooseEPSDestFileTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXEPSPrintSetupDialog;

class JXChooseEPSDestFileTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXChooseEPSDestFileTask(JXEPSPrintSetupDialog* director);

	virtual ~JXChooseEPSDestFileTask();

	virtual void	Perform();

private:

	JXEPSPrintSetupDialog*	itsDirector;	// not owned

private:

	// not allowed

	JXChooseEPSDestFileTask(const JXChooseEPSDestFileTask& source);
	const JXChooseEPSDestFileTask& operator=(const JXChooseEPSDestFileTask& source);
};

#endif
