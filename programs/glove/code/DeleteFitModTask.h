/******************************************************************************
 DeleteFitModTask.h

	Interface for the DeleteFitModTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_DeleteFitModTask
#define _H_DeleteFitModTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
