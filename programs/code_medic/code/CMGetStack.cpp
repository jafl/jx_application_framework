/******************************************************************************
 CMGetStack.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMGetStack.h"
#include "CMStackWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetStack::CMGetStack
	(
	const JString&	cmd,
	JTree*			tree,
	CMStackWidget*	widget
	)
	:
	CMCommand(cmd, false, true),
	itsTree(tree),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetStack::~CMGetStack()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetStack::HandleFailure()
{
	itsWidget->FinishedLoading(0);
}
