/******************************************************************************
 JXAdjustCompartmentsTask.h

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustCompartmentsTask
#define _H_JXAdjustCompartmentsTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>
#include <JArray.h>

class JXPartition;

class JXAdjustCompartmentsTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXAdjustCompartmentsTask(JXPartition* partition, JArray<JCoordinate>* sizes);

	virtual ~JXAdjustCompartmentsTask();

	virtual void	Perform();

private:

	JXPartition*			itsPartition;	// not owned
	JArray<JCoordinate>*	itsSizes;

private:

	// not allowed

	JXAdjustCompartmentsTask(const JXAdjustCompartmentsTask& source);
	const JXAdjustCompartmentsTask& operator=(const JXAdjustCompartmentsTask& source);
};

#endif
