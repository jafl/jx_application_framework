/******************************************************************************
 GDBGetStopLocationForLink.cpp

	BASE CLASS = GDBGetStopLocation

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBGetStopLocationForLink.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStopLocationForLink::GDBGetStopLocationForLink()
	:
	GDBGetStopLocation()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStopLocationForLink::~GDBGetStopLocationForLink()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetStopLocationForLink::HandleSuccess
	(
	const JString& cmdData
	)
{
	dynamic_cast<GDBLink*>(CMGetLink())->SendProgramStopped(GetLocation());
}
