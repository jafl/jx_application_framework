/******************************************************************************
 CMBreakpoint.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "CMBreakpoint.h"
#include "CMBreakpointManager.h"
#include "CMGetFullPath.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMBreakpoint::CMBreakpoint
	(
	const JSize			debuggerIndex,
	const CMLocation&	location,
	const JCharacter*	fn,
	const JCharacter*	addr,
	const JBoolean		enabled,
	const Action		action,
	const JCharacter*	condition,
	const JSize			ignoreCount
	)
	:
	itsDebuggerIndex(debuggerIndex),
	itsLocation(location),
	itsFnName(fn),
	itsAddr(addr),
	itsEnabledFlag(enabled),
	itsAction(action),
	itsCondition(condition),
	itsIgnoreCount(ignoreCount)
{
	CMBreakpointX();
}

CMBreakpoint::CMBreakpoint
	(
	const JSize			debuggerIndex,
	const JCharacter*	fileName,
	const JSize			lineIndex,
	const JCharacter*	fn,
	const JCharacter*	addr,
	const JBoolean		enabled,
	const Action		action,
	const JCharacter*	condition,
	const JSize			ignoreCount
	)
	:
	itsDebuggerIndex(debuggerIndex),
	itsLocation(fileName, lineIndex),
	itsFnName(fn),
	itsAddr(addr),
	itsEnabledFlag(enabled),
	itsAction(action),
	itsCondition(condition),
	itsIgnoreCount(ignoreCount)
{
	CMBreakpointX();
}

// search target

CMBreakpoint::CMBreakpoint
	(
	const JCharacter* fileName,
	const JIndex lineIndex
	)
	:
	itsDebuggerIndex(0),
	itsLocation(fileName, lineIndex),
	itsEnabledFlag(kJTrue),
	itsAction(kKeepBreakpoint),
	itsIgnoreCount(0)
{
	CMBreakpointX();
}

// private

void
CMBreakpoint::CMBreakpointX()
{
	const JString& fileName = GetFileName();
	if (!fileName.IsEmpty())	// search target may have empty file name
		{
		JBoolean exists;
		JString fullName;
		if ((CMGetLink())->FindFile(fileName, &exists, &fullName))
			{
			if (exists)
				{
				itsLocation.SetFileName(fullName);
				}
			}
		else
			{
			CMGetFullPath* cmd = (CMGetLink())->CreateGetFullPath(fileName);
			ListenTo(cmd);
			}
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMBreakpoint::~CMBreakpoint()
{
}

/******************************************************************************
 DisplayStatus

 *****************************************************************************/

void
CMBreakpoint::DisplayStatus()
	const
{
	(CMGetLink())->ShowBreakpointInfo(itsDebuggerIndex);
}

/******************************************************************************
 ToggleEnabled

 *****************************************************************************/

void
CMBreakpoint::ToggleEnabled()
{
	(CMGetLink())->SetBreakpointEnabled(itsDebuggerIndex, !itsEnabledFlag);
}

/******************************************************************************
 RemoveCondition

 *****************************************************************************/

void
CMBreakpoint::RemoveCondition()
{
	(CMGetLink())->RemoveBreakpointCondition(itsDebuggerIndex);
}

/******************************************************************************
 SetIgnoreCount

 *****************************************************************************/

void
CMBreakpoint::SetIgnoreCount
	(
	const JSize count
	)
{
	(CMGetLink())->SetBreakpointIgnoreCount(itsDebuggerIndex, count);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMBreakpoint::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(CMGetFullPath::kFileFound))
		{
		const CMGetFullPath::FileFound* info =
			dynamic_cast<const CMGetFullPath::FileFound*>(&message);
		assert( info != NULL );
		itsLocation.SetFileName(info->GetFullName());
		(CMGetLink()->GetBreakpointManager())->BreakpointFileNameResolved(this);
		}
	else if (message.Is(CMGetFullPath::kFileNotFound))
		{
		(CMGetLink()->GetBreakpointManager())->BreakpointFileNameInvalid(this);
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
