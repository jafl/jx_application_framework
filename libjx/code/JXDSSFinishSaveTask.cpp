/******************************************************************************
 JXDSSFinishSaveTask.cpp

	BASE CLASS = JXDirectSaveAction, JXUrgentTask

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXDSSFinishSaveTask.h>
#include <JXSaveFileDialog.h>
#include <jDirUtil.h>
#include <jAssert.h>

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

	itsDirName = jnew JString;
	assert( itsDirName != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDSSFinishSaveTask::~JXDSSFinishSaveTask()
{
	jdelete itsDirName;
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
	JSplitPathAndName(fullName, itsDirName, &name);
	Go();
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
JXDSSFinishSaveTask::Perform()
{
	itsDialog->Save(*itsDirName);
}
