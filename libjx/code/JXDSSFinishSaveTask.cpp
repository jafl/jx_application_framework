/******************************************************************************
 JXDSSFinishSaveTask.cpp

	BASE CLASS = JXDirectSaveAction, JXUrgentTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXDSSFinishSaveTask.h"
#include "JXSaveFileDialog.h"
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDSSFinishSaveTask::JXDSSFinishSaveTask
	(
	JXSaveFileDialog* dialog
	)
	:
	JXDirectSaveAction(),
	JXUrgentTask(dialog),
	itsDialog(dialog)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXDSSFinishSaveTask::~JXDSSFinishSaveTask()
{
}

/******************************************************************************
 Save (virtual)

 ******************************************************************************/

void
JXDSSFinishSaveTask::Save
	(
	const JString& fullName
	)
{
	JString name;
	JSplitPathAndName(fullName, &itsDirName, &name);
	Go();
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXDSSFinishSaveTask::Perform()
{
	if (!itsDirName.IsEmpty())
	{
		itsDialog->Save(itsDirName);
	}
}
