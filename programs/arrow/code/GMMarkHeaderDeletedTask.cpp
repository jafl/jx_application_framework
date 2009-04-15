/******************************************************************************
 GMMarkHeaderDeletedTask.cc

	<Description>

	BASE CLASS = public JXUrgentTask

	Copyright © 2003 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GMMarkHeaderDeletedTask.h>
#include "GMessageHeader.h"
#include "GMMailboxData.h"

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GMMarkHeaderDeletedTask::GMMarkHeaderDeletedTask
	(
	GMMailboxData* 	data, 
	GMessageHeader*	header
	)
	:
	JXUrgentTask(),
	itsData(data),
	itsHeader(header)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMMarkHeaderDeletedTask::~GMMarkHeaderDeletedTask()
{
}

/******************************************************************************
 Perform (virtual public)

 *****************************************************************************/

void
GMMarkHeaderDeletedTask::Perform()
{
	itsData->ChangeMessageStatus(itsHeader, GMessageHeader::kDelete);
}

