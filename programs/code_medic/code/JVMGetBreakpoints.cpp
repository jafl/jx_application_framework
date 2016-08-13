/******************************************************************************
 JVMGetBreakpoints.cpp

	BASE CLASS = CMGetBreakpoints

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JVMGetBreakpoints.h"
#include "JVMLink.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetBreakpoints::JVMGetBreakpoints()
	:
	CMGetBreakpoints("NOP")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetBreakpoints::~JVMGetBreakpoints()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
JVMGetBreakpoints::HandleSuccess
	(
	const JString& data
	)
{
}
