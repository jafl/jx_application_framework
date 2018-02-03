/******************************************************************************
 CMAdjustLineTableToTextTask.cpp

	AdjustToText() cannot be called by CMLineIndexTable ctor.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "CMAdjustLineTableToTextTask.h"
#include "CMLineIndexTable.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMAdjustLineTableToTextTask::CMAdjustLineTableToTextTask
	(
	CMLineIndexTable* table
	)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMAdjustLineTableToTextTask::~CMAdjustLineTableToTextTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMAdjustLineTableToTextTask::Perform()
{
	itsTable->AdjustToText();
}
