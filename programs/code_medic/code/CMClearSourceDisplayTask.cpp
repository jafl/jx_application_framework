/******************************************************************************
 CMClearSourceDisplayTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "CMClearSourceDisplayTask.h"
#include "CMSourceDirector.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMClearSourceDisplayTask::CMClearSourceDisplayTask
	(
	CMSourceDirector* dir
	)
{
	itsDirector = dir;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMClearSourceDisplayTask::~CMClearSourceDisplayTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMClearSourceDisplayTask::Perform()
{
	itsDirector->ClearDisplay();
}
