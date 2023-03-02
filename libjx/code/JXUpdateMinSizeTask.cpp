/******************************************************************************
 JXUpdateMinSizeTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "JXUpdateMinSizeTask.h"
#include "JXDockWidget.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUpdateMinSizeTask::JXUpdateMinSizeTask
	(
	JXDockWidget* dock
	)
	:
	JXUrgentTask(dock),
	itsDockWidget(dock)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXUpdateMinSizeTask::~JXUpdateMinSizeTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXUpdateMinSizeTask::Perform()
{
	itsDockWidget->UpdateMinSize();
}
