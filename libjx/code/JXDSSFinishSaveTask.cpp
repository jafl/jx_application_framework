/******************************************************************************
 JXDSSFinishSaveTask.cpp

	BASE CLASS = JXDirectSaveAction, JXUrgentTask

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDSSFinishSaveTask.h>
#include <JXSaveFileDialog.h>
#include <jXGlobals.h>
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

	itsDirName = new JString;
	assert( itsDirName != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDSSFinishSaveTask::~JXDSSFinishSaveTask()
{
	delete itsDirName;
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

	(JXGetApplication())->InstallUrgentTask(this);
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
JXDSSFinishSaveTask::Perform()
{
	itsDialog->Save(*itsDirName);
}
