/******************************************************************************
 JXAdjustPrintSetupLayoutTask.h

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustPrintSetupLayoutTask
#define _H_JXAdjustPrintSetupLayoutTask

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXDirector;
class JXWidget;

class JXAdjustPrintSetupLayoutTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXAdjustPrintSetupLayoutTask(JXDirector* director, JXWidget* printCmd,
								   JXWidget* chooseFileButton, JXWidget* fileInput);

	~JXAdjustPrintSetupLayoutTask();

	void	Perform() override;

private:

	JXDirector*	itsDirector;			// not owned
	JXWidget*	itsPrintCmd;			// not owned
	JXWidget*	itsChooseFileButton;	// not owned
	JXWidget*	itsFileInput;			// not owned
};

#endif
