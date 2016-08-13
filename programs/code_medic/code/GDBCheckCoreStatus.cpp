/******************************************************************************
 GDBCheckCoreStatus.cpp

	BASE CLASS = CMCommand

	Copyright © 2001 by John Lindal. All rights reserved.

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
	CMCommand("info files", kJTrue, kJFalse)
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

#define OSX_CORE_PATH "/cores/"

static const JRegex coreFilePattern = "Local core dump file:\n\t`([^\n]+)'";
static const JRegex osxFilePattern  = "section-info=\\[filename=\"" OSX_CORE_PATH "([^\"]+)\"";

void
GDBCheckCoreStatus::HandleSuccess
	(
	const JString& data
	)
{
	JString fileName;

	JArray<JIndexRange> matchList;
	if (coreFilePattern.Match(data, &matchList))
		{
		fileName = data.GetSubstring(matchList.GetElement(2));
		}
	else if (osxFilePattern.Match(GetLastResult(), &matchList))
		{
		fileName = OSX_CORE_PATH + data.GetSubstring(matchList.GetElement(2));
		}
	else
		{
		(CMGetLink())->Log("GDBCheckCoreStatus failed to match (ok if there is no core file)");
		}

	// CMLink has to broadcast status of core regardless of whether or not
	// we get what we expect from gdb.

	dynamic_cast<GDBLink*>(CMGetLink())->SaveCoreName(fileName);
}
