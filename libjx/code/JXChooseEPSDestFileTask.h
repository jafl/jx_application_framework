/******************************************************************************
 JXChooseEPSDestFileTask.h

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseEPSDestFileTask
#define _H_JXChooseEPSDestFileTask

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXEPSPrintSetupDialog;

class JXChooseEPSDestFileTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXChooseEPSDestFileTask(JXEPSPrintSetupDialog* director);

	~JXChooseEPSDestFileTask();

	void	Perform() override;

private:

	JXEPSPrintSetupDialog*	itsDirector;	// not owned
};

#endif
