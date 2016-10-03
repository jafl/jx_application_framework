/******************************************************************************
 XDGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "XDGetFrame.h"
#include "CMStackWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetFrame::XDGetFrame
	(
	CMStackWidget* widget
	)
	:
	CMGetFrame("status"),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetFrame::~XDGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetFrame::HandleSuccess
	(
	const JString& data
	)
{
	itsWidget->SelectFrame(0);
}
