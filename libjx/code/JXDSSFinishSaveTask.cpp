/******************************************************************************
 JXDSSFinishSaveTask.cpp

	BASE CLASS = JXDirectSaveAction, JXUrgentTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXDSSFinishSaveTask.h"
#include "jx-af/jx/JXSaveFileDialog.h"
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
	JXUrgentTask()
{
	itsDialog = dialog;
}

/******************************************************************************
 Destructor

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
 Perform (virtual)

 ******************************************************************************/

void
JXDSSFinishSaveTask::Perform()
{
	itsDialog->Save(itsDirName);
}
