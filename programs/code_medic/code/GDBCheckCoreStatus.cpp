/******************************************************************************
 GDBCheckCoreStatus.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBCheckCoreStatus.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBCheckCoreStatus::GDBCheckCoreStatus()
	:
	CMCommand("info files", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBCheckCoreStatus::~GDBCheckCoreStatus()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex coreFilePattern = "Local core dump file:\n\t`([^\n]+)'";

void
GDBCheckCoreStatus::HandleSuccess
	(
	const JString& data
	)
{
	JString fileName;

	const JStringMatch m = coreFilePattern.Match(data, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		fileName = m.GetSubstring(1);
		}
	else
		{
		CMGetLink()->Log("GDBCheckCoreStatus failed to match (ok if there is no core file)");
		}

	// CMLink has to broadcast status of core regardless of whether or not
	// we get what we expect from gdb.

	dynamic_cast<GDBLink*>(CMGetLink())->SaveCoreName(fileName);
}
