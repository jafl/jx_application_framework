/******************************************************************************
 GDBGetStopLocation.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2009-11 by John Lindal.

 ******************************************************************************/

#include "GDBGetStopLocation.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetStopLocation::GDBGetStopLocation()
	:
	CMCommand("-stack-info-frame", kJFalse, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetStopLocation::~GDBGetStopLocation()
{
}

/******************************************************************************
 GetLocation (protected)

 ******************************************************************************/

static const JRegex locationPattern = "\\bframe=\\{";

CMLocation
GDBGetStopLocation::GetLocation()
	const
{
	const JString& data = GetLastResult();

	CMLocation loc;
	JIndexRange r;
	if (locationPattern.Match(data, &r))
		{
		std::istringstream stream(data.GetCString());
		stream.seekg(r.last);

		JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
		JString *s, *s1, *fullName;
		JIndex lineIndex;
		const JBoolean parsed = GDBLink::ParseMap(stream, &map);
		if (!parsed)
			{
			CMGetLink()->Log("invalid data map");
			}
		else if (!map.GetElement("fullname", &fullName))
			{
			CMGetLink()->Log("missing file name");
			}
		else if (!map.GetElement("line", &s))
			{
			CMGetLink()->Log("missing line index");
			}
		else if (!s->ConvertToUInt(&lineIndex))
			{
			CMGetLink()->Log("line index is not integer");
			}
		else
			{
			loc.SetFileName(*fullName);
			loc.SetLineNumber(lineIndex);
			}

		if (parsed && map.GetElement("func", &s) && map.GetElement("addr", &s1))
			{
			loc.SetFunctionName(*s);
			loc.SetMemoryAddress(*s1);
			}
		}
	else
		{
		CMGetLink()->Log("GDBGetStopLocation failed to match");
		}

	return loc;
}
