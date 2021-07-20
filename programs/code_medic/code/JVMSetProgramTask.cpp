/******************************************************************************
 JVMSetProgramTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMSetProgramTask.h"
#include "JVMLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMSetProgramTask::JVMSetProgramTask()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMSetProgramTask::~JVMSetProgramTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JVMSetProgramTask::Perform()
{
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	if (link != nullptr)
		{
		link->BroadcastProgramSet();
		}
}
