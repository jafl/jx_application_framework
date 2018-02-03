/******************************************************************************
 CMUpdateCommandsTableWidth.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "CMUpdateCommandsTableWidth.h"
#include "CMEditCommandsTable.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMUpdateCommandsTableWidth::CMUpdateCommandsTableWidth
	(
	CMEditCommandsTable* table
	)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMUpdateCommandsTableWidth::~CMUpdateCommandsTableWidth()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
CMUpdateCommandsTableWidth::Perform()
{
	itsTable->AdjustColWidth();
}
