/******************************************************************************
 GDBGetBreakpoints.cpp

	This is the only way to get all the relevant information about each
	breakpoint.  gdb does not print enough information when "break" is
	used.

	BASE CLASS = CMGetBreakpoints

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetBreakpoints.h"
#include "CMBreakpointManager.h"
#include "GDBLink.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetBreakpoints::GDBGetBreakpoints()
	:
	CMGetBreakpoints("-break-list")
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetBreakpoints::~GDBGetBreakpoints()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

static const JRegex bpPattern = "\\bbkpt=\\{";

void
GDBGetBreakpoints::HandleSuccess
	(
	const JString& cmdData
	)
{
	(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJFalse);

	JPtrArray<CMBreakpoint> bpList(JPtrArrayT::kForgetAll);		// ownership taken by CMBreakpointManager
	bpList.SetCompareFunction(CMBreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JOrderedSetT::kSortAscending);

	JPtrArray<CMBreakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager

	const JString& data = GetLastResult();
	std::istringstream stream(data.GetCString());

	JIndexRange origRange, newRange;
	JStringPtrMap<JString> map(JPtrArrayT::kDeleteAll);
	while (bpPattern.MatchAfter(data, origRange, &newRange))
		{
		stream.seekg(newRange.last);
		if (!GDBLink::ParseMap(stream, &map))
			{
			(CMGetLink())->Log("invalid data map");
			break;
			}
		origRange.first = origRange.last = ((std::streamoff) stream.tellg()) + 1;

		JString* s;
		if (!map.GetElement("type", &s))
			{
			(CMGetLink())->Log("missing breakpoint type");
			}
		else if (*s == "breakpoint")
			{
			ParseBreakpoint(map, &bpList);
			}
		else
			{
			ParseOther(map, &otherList);
			}
		}

	(CMGetLink()->GetBreakpointManager())->UpdateBreakpoints(bpList, otherList);
}

/******************************************************************************
 ParseBreakpoint (private)

 *****************************************************************************/

void
GDBGetBreakpoints::ParseBreakpoint
	(
	JStringPtrMap<JString>&		map,
	JPtrArray<CMBreakpoint>*	list
	)
{
	JIndex bpIndex;
	CMBreakpoint::Action action;
	JBoolean enabled;
	JSize ignoreCount;
	if (!ParseCommon(map, &bpIndex, &action, &enabled, &ignoreCount))
		{
		return;
		}

	JString* s;
	JString fileName;
	JIndex lineIndex;
	do
	{
		if (!map.GetElement("line", &s))
			{
			(CMGetLink())->Log("warn: missing breakpoint line");
			break;
			}
		if (!s->ConvertToUInt(&lineIndex))
			{
			(CMGetLink())->Log("warn: line number is not integer");
			break;
			}

		if (!map.GetElement("file", &s))
			{
			(CMGetLink())->Log("warn: missing breakpoint filename");
			break;
			}
		fileName = *s;
	}
	while (0);

	do
	{
		if (!fileName.IsEmpty())
			{
			break;
			}

		if (map.GetElement("original-location", &s))
			{
			JIndex i;
			if (!s->LocateSubstring(":", &i))
				{
				(CMGetLink())->Log("warn: missing line number in original-location");
				break;
				}

			if (1 < i && i < s->GetLength())
				{
				JString n = s->GetSubstring(i+1, s->GetLength());
				if (!n.ConvertToUInt(&lineIndex))
					{
					(CMGetLink())->Log("warn: line number is not integer in original-location");
					break;
					}

				fileName = s->GetSubstring(1, i-1);
				}
			}
	}
	while (0);

	if (fileName.IsEmpty())
		{
		(CMGetLink())->Log("unable to parse breakpoint location");
		}

	JString fn;
	if (map.GetElement("func", &s))
		{
		fn = *s;
		}

	JString addr;
	if (map.GetElement("addr", &s))
		{
		addr = *s;
		}

	JString condition;
	if (map.GetElement("cond", &s))
		{
		condition = *s;
		}

	CMBreakpoint* bp =
		jnew CMBreakpoint(bpIndex, fileName, lineIndex, fn, addr,
						 enabled, action, condition, ignoreCount);
	assert( bp != NULL );
	list->InsertSorted(bp);
}

/******************************************************************************
 ParseOther (private)

 *****************************************************************************/

void
GDBGetBreakpoints::ParseOther
	(
	JStringPtrMap<JString>&		map,
	JPtrArray<CMBreakpoint>*	list
	)
{
	JIndex bpIndex;
	CMBreakpoint::Action action;
	JBoolean enabled;
	JSize ignoreCount;
	if (!ParseCommon(map, &bpIndex, &action, &enabled, &ignoreCount))
		{
		return;
		}

	JString* s;
	JString fn;
	if (map.GetElement("type", &s))
		{
		fn = *s;
		}

	JString condition;
	if (map.GetElement("what", &s))
		{
		condition = *s;
		}

	CMBreakpoint* bp =
		jnew CMBreakpoint(bpIndex, CMLocation(), fn, "",
						 enabled, action, condition, ignoreCount);
	assert( bp != NULL );
	list->Append(bp);

	if (fn.Contains("watchpoint"))		// may be deleted when go out of scope
		{
		(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJTrue);
		}
}

/******************************************************************************
 ParseCommon (private)

 *****************************************************************************/

JBoolean
GDBGetBreakpoints::ParseCommon
	(
	JStringPtrMap<JString>&	map,
	JIndex*					bpIndex,
	CMBreakpoint::Action*	action,
	JBoolean*				enabled,
	JSize*					ignoreCount
	)
{
	JString* s;
	if (!map.GetElement("number", &s))
		{
		(CMGetLink())->Log("missing otherpoint number");
		return kJFalse;
		}
	if (!s->ConvertToUInt(bpIndex))
		{
		(CMGetLink())->Log("otherpoint number is not integer");
		return kJFalse;
		}

	if (!map.GetElement("disp", &s))
		{
		(CMGetLink())->Log("missing otherpoint action");
		return kJFalse;
		}
	if (JStringCompare(*s, "del", kJFalse) == 0)
		{
		*action = CMBreakpoint::kRemoveBreakpoint;
		}
	else if (JStringCompare(*s, "dis", kJFalse) == 0)
		{
		*action = CMBreakpoint::kDisableBreakpoint;
		}
	else	// "keep" or unknown
		{
		*action = CMBreakpoint::kKeepBreakpoint;
		}

	if (!map.GetElement("enabled", &s))
		{
		(CMGetLink())->Log("missing otherpoint enable status");
		return kJFalse;
		}
	*enabled = JI2B(*s == "y" || *s == "Y");

	*ignoreCount = 0;
	if (map.GetElement("ignore", &s) && !s->IsEmpty() &&
		!s->ConvertToUInt(ignoreCount))
		{
		(CMGetLink())->Log("ignore count is not integer");
		return kJFalse;
		}

	// may be deleted or other status change

	if (*action != CMBreakpoint::kKeepBreakpoint || *ignoreCount > 0)
		{
		(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJTrue);
		}

	return kJTrue;
}
