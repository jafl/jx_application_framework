/******************************************************************************
 JXChooseEPSDestFileTask.h

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseEPSDestFileTask
#define _H_JXChooseEPSDestFileTask

#include "JXUrgentTask.h"

class JXEPSPrintSetupDialog;

class JXChooseEPSDestFileTask : public JXUrgentTask
{
public:

	JXChooseEPSDestFileTask(JXEPSPrintSetupDialog* director);

protected:

	~JXChooseEPSDestFileTask() override;

	void	Perform() override;

private:

	JXEPSPrintSetupDialog*	itsDirector;	// not owned
};

#endif
