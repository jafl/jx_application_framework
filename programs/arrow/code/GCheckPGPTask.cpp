/******************************************************************************
 GCheckPGPTask.cc

	BASE CLASS = JXUrgentTask

	Copyright © 1996 by John Lindal. All rights reserved.

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
