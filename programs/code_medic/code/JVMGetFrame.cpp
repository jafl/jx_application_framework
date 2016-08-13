/******************************************************************************
 JVMGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JVMGetFrame.h"
#include "CMStackWidget.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetFrame::JVMGetFrame
	(
	CMStackWidget* widget
	)
	:
	CMGetFrame("NOP"),
	itsWidget(widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetFrame::~JVMGetFrame()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetFrame::HandleSuccess
	(
	const JString& data
	)
{
}
