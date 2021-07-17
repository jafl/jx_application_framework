/******************************************************************************
 JVMGetFrame.cpp

	BASE CLASS = CMGetFrame

	Copyright (C) 2009 by John Lindal.

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
	CMGetFrame(JString("NOP", JString::kNoCopy)),
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
