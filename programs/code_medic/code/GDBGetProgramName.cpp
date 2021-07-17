/******************************************************************************
 GDBGetProgramName.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetProgramName.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetProgramName::GDBGetProgramName()
	:
	CMCommand("info files", true, false)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetProgramName::~GDBGetProgramName()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex fileNamePattern = "Local exec file:\n\t`([^\n]+)'";

void
GDBGetProgramName::HandleSuccess
	(
	const JString& data
	)
{
	JString fileName;

	const JStringMatch m = fileNamePattern.Match(data, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		fileName = m.GetSubstring(1);
		}
	else
		{
		CMGetLink()->Log("GDBGetProgramName failed to match");
		}

	// CMLink has to broadcast SymbolsLoaded regardless of whether or not
	// we get what we expect from gdb.

	dynamic_cast<GDBLink*>(CMGetLink())->SaveProgramName(fileName);
}
