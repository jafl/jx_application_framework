/******************************************************************************
 GDBGetStopLocationForAsm.cpp

	BASE CLASS = GDBGetStopLocation

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetStopLocationForAsm.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStopLocationForAsm::GDBGetStopLocationForAsm()
	:
	GDBGetStopLocation()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStopLocationForAsm::~GDBGetStopLocationForAsm()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetStopLocationForAsm::HandleSuccess
	(
	const JString& cmdData
	)
{
	dynamic_cast<GDBLink*>(CMGetLink())->SendProgramStopped2(GetLocation());
}
