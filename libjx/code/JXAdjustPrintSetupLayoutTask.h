/******************************************************************************
 JXAdjustPrintSetupLayoutTask.h

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustPrintSetupLayoutTask
#define _H_JXAdjustPrintSetupLayoutTask

#include "JXUrgentTask.h"

class JXDirector;
class JXWidget;

class JXAdjustPrintSetupLayoutTask : public JXUrgentTask
{
public:

	JXAdjustPrintSetupLayoutTask(JXDirector* director, JXWidget* printCmd,
								 JXWidget* chooseFileButton, JXWidget* fileInput);

protected:

	~JXAdjustPrintSetupLayoutTask() override;

	void	Perform() override;

private:

	JXWidget*	itsPrintCmd;			// not owned
	JXWidget*	itsChooseFileButton;	// not owned
	JXWidget*	itsFileInput;			// not owned
};

#endif
