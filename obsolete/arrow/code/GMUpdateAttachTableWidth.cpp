/******************************************************************************
 GMUpdateAttachTableWidth.cc

	BASE CLASS = JXUrgentTask

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GMUpdateAttachTableWidth.h>
#include <GMAttachmentTable.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMUpdateAttachTableWidth::GMUpdateAttachTableWidth
	(
	GMAttachmentTable* table
	)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMUpdateAttachTableWidth::~GMUpdateAttachTableWidth()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GMUpdateAttachTableWidth::Perform()
{
	itsTable->AdjustColWidth();
}
