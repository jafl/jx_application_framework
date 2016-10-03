/******************************************************************************
 DeleteFitModTask.h

	Interface for the DeleteFitModTask class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_DeleteFitModTask
#define _H_DeleteFitModTask

#include <JXUrgentTask.h>

class FitModule;

class DeleteFitModTask : public JXUrgentTask
{
public:

	DeleteFitModTask(FitModule* module);

	virtual ~DeleteFitModTask();

	virtual void	Perform();

private:

	FitModule* 		itsModule;

	// not allowed

	DeleteFitModTask(const DeleteFitModTask& source);
	const DeleteFitModTask& operator=(const DeleteFitModTask& source);
};

#endif
