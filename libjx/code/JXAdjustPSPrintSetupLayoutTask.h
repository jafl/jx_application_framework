/******************************************************************************
 JXAdjustPSPrintSetupLayoutTask.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAdjustPSPrintSetupLayoutTask
#define _H_JXAdjustPSPrintSetupLayoutTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXDirector;
class JXWidget;

class JXAdjustPSPrintSetupLayoutTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXAdjustPSPrintSetupLayoutTask(JXDirector* director, const JCoordinate delta,
								   JXWidget* chooseFileButton, JXWidget* fileInput);

	virtual ~JXAdjustPSPrintSetupLayoutTask();

	virtual void	Perform();

private:

	JXDirector*			itsDirector;			// not owned
	const JCoordinate	itsDelta;
	JXWidget*			itsChooseFileButton;	// not owned
	JXWidget*			itsFileInput;			// not owned

private:

	// not allowed

	JXAdjustPSPrintSetupLayoutTask(const JXAdjustPSPrintSetupLayoutTask& source);
	const JXAdjustPSPrintSetupLayoutTask& operator=(const JXAdjustPSPrintSetupLayoutTask& source);
};

#endif
