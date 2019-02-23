/******************************************************************************
 SyGBeginEditingTask.cpp

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "SyGBeginEditingTask.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTreeList.h"
#include "SyGFileTreeNode.h"
#include <jXConstants.h>
#include <JTableSelection.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGBeginEditingTask::SyGBeginEditingTask
	(
	SyGFileTreeTable*	table,
	const JPoint&		cell
	)
	:
	JXIdleTask(kJXDoubleClickTime),
	itsTable(table),
	itsNode(table->GetFileTreeList()->GetSyGNode(cell.y))
{
	ClearWhenGoingAway(itsNode, &itsNode);
	ClearWhenGoingAway(itsNode, &(itsTable->itsEditTask));

	ClearWhenGoingAway(itsTable, &itsNode);
	ClearWhenGoingAway(itsTable, &itsTable);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGBeginEditingTask::~SyGBeginEditingTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
SyGBeginEditingTask::Perform()
{
	if (itsTable != nullptr)
		{
		itsTable->itsEditTask = nullptr;	// first action:  allows it to create another one, if necessary

		JPoint cell;
		if (itsTable->GetTableSelection().GetSingleSelectedCell(&cell) &&
			itsTable->GetFileTreeList()->GetSyGNode(cell.y) == itsNode)
			{
			itsTable->BeginEditing(cell);
			itsTable->TableScrollToCell(JPoint(itsTable->GetToggleOpenColIndex(), cell.y));
			}
		}

	jdelete this;
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
SyGBeginEditingTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		Perform();
		}
}
