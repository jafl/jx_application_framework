/******************************************************************************
 SVNBeginEditingTask.cpp

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 2000-08 by John Lindal.

 ******************************************************************************/

#include "SVNBeginEditingTask.h"
#include "SVNRepoView.h"
#include "SVNRepoTreeList.h"
#include "SVNRepoTreeNode.h"
#include <jXConstants.h>
#include <JTableSelection.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNBeginEditingTask::SVNBeginEditingTask
	(
	SVNRepoView*		view,
	const JPoint&		cell
	)
	:
	JXIdleTask(kJXDoubleClickTime),
	itsView(view),
	itsNode((view->GetRepoTreeList())->GetRepoNode(cell.y))
{
	ClearWhenGoingAway(itsNode, &itsNode);
	ClearWhenGoingAway(itsNode, &(itsView->itsEditTask));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNBeginEditingTask::~SVNBeginEditingTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
SVNBeginEditingTask::Perform()
{
	itsView->itsEditTask = nullptr;	// first action:  allows it to create another one, if necessary

	JPoint cell;
	if ((itsView->GetTableSelection()).GetSingleSelectedCell(&cell) &&
		(itsView->GetRepoTreeList())->GetRepoNode(cell.y) == itsNode)
		{
		itsView->BeginEditing(cell);
		itsView->TableScrollToCell(JPoint(itsView->GetToggleOpenColIndex(), cell.y));
		}

	jdelete this;
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
SVNBeginEditingTask::Perform
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
