/******************************************************************************
 JXFocusWidgetTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "JXFocusWidgetTask.h"
#include "JXWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Focus (static)

 ******************************************************************************/

void
JXFocusWidgetTask::Focus
	(
	JXWidget* widget
	)
{
	auto* task = jnew JXFocusWidgetTask(widget);
	task->Start();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFocusWidgetTask::JXFocusWidgetTask
	(
	JXWidget* widget
	)
	:
	JXIdleTask(1),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXFocusWidgetTask::~JXFocusWidgetTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXFocusWidgetTask::Perform
	(
	const Time delta
	)
{
	itsWidget->Focus();
	jdelete this;
}
