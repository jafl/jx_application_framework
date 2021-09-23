/******************************************************************************
 JXAdjustPrintSetupLayoutTask.h

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustPrintSetupLayoutTask
#define _H_JXAdjustPrintSetupLayoutTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXDirector;
class JXWidget;

class JXAdjustPrintSetupLayoutTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXAdjustPrintSetupLayoutTask(JXDirector* director, JXWidget* printCmd,
								   JXWidget* chooseFileButton, JXWidget* fileInput);

	virtual ~JXAdjustPrintSetupLayoutTask();

	virtual void	Perform();

private:

	JXDirector*	itsDirector;			// not owned
	JXWidget*	itsPrintCmd;			// not owned
	JXWidget*	itsChooseFileButton;	// not owned
	JXWidget*	itsFileInput;			// not owned
};

#endif
