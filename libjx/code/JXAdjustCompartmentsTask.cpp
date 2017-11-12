/******************************************************************************
 JXAdjustCompartmentsTask.cpp

	Sets the sizes of all the compartments.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXAdjustCompartmentsTask.h>
#include <JXPartition.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustCompartmentsTask::JXAdjustCompartmentsTask
	(
	JXPartition*			partition,
	JArray<JCoordinate>*	sizes
	)
	:
	itsPartition(partition),
	itsSizes(sizes)
{
	ClearWhenGoingAway(itsPartition, &itsPartition);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAdjustCompartmentsTask::~JXAdjustCompartmentsTask()
{
	jdelete itsSizes;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXAdjustCompartmentsTask::Perform()
{
	if (itsPartition != NULL)
		{
		itsPartition->SetCompartmentSizes(*itsSizes);
		}
}
