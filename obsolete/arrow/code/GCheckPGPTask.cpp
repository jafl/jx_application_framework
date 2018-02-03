/******************************************************************************
 GCheckPGPTask.cc

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <GCheckPGPTask.h>
#include <GMGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GCheckPGPTask::GCheckPGPTask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GCheckPGPTask::~GCheckPGPTask()
{
}

/******************************************************************************
 Perform

	By clearing itsAdjustBoundsTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
GCheckPGPTask::Perform()
{
	GCheckPGP();
}
