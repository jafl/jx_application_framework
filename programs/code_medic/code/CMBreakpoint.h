/******************************************************************************
 CMBreakpoint.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMBreakpoint
#define _H_CMBreakpoint

#include "CMLocation.h"

class CMLink;

class CMBreakpoint : virtual public JBroadcaster
{
public:

	enum Action
	{
		// Do not change these values after they are assigned,
		// because they are stored in debug_config

		kKeepBreakpoint = 0,
		kRemoveBreakpoint,
		kDisableBreakpoint
	};

public:

	CMBreakpoint(const JIndex debuggerIndex, const CMLocation& location,
				 const JCharacter* fn, const JCharacter* addr,
				 const JBoolean enabled, const Action action,
				 const JCharacter* condition, const JSize ignoreCount);

	CMBreakpoint(const JIndex debuggerIndex,
				 const JCharacter* fileName, const JIndex lineIndex,
				 const JCharacter* fn, const JCharacter* addr,
				 const JBoolean enabled, const Action action,
				 const JCharacter* condition, const JSize ignoreCount);

	// search target
	CMBreakpoint(const JCharacter* fileName, const JIndex lineIndex);
	CMBreakpoint(const JCharacter* addr);

	virtual	~CMBreakpoint();

	JIndex				GetDebuggerIndex() const;
	const CMLocation&	GetLocation() const;
	const JString&		GetFileName() const;
	const JFileID&		GetFileID() const;
	JIndex				GetLineNumber() const;
	const JString&		GetFunctionName() const;
	const JString&		GetAddress() const;
	JBoolean			IsEnabled() const;
	Action				GetAction() const;
	JBoolean			HasCondition() const;
	JBoolean			GetCondition(JString* condition) const;
	JSize				GetIgnoreCount() const;

	void	DisplayStatus() const;
	void	ToggleEnabled();
	void	RemoveCondition();
	void	SetIgnoreCount(const JSize count);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex		itsDebuggerIndex;
	CMLocation	itsLocation;
	JString		itsFnName;
	JString		itsAddr;
	JBoolean	itsEnabledFlag;
	Action		itsAction;
	JString		itsCondition;
	JSize		itsIgnoreCount;

private:

	void	CMBreakpointX();
};


/******************************************************************************
 Breakpoint index

 *****************************************************************************/

inline JIndex
CMBreakpoint::GetDebuggerIndex()
	const
{
	return itsDebuggerIndex;
}

/******************************************************************************
 Location

 *****************************************************************************/

inline const CMLocation&
CMBreakpoint::GetLocation()
	const
{
	return itsLocation;
}

/******************************************************************************
 File name

 *****************************************************************************/

inline const JString&
CMBreakpoint::GetFileName()
	const
{
	return itsLocation.GetFileName();
}

/******************************************************************************
 File ID

 *****************************************************************************/

inline const JFileID&
CMBreakpoint::GetFileID()
	const
{
	return itsLocation.GetFileID();
}

/******************************************************************************
 Line number

 *****************************************************************************/

inline JSize
CMBreakpoint::GetLineNumber()
	const
{
	return itsLocation.GetLineNumber();
}

/******************************************************************************
 Function name

 *****************************************************************************/

inline const JString&
CMBreakpoint::GetFunctionName()
	const
{
	return itsFnName;
}

/******************************************************************************
 Address (string)

 *****************************************************************************/

inline const JString&
CMBreakpoint::GetAddress()
	const
{
	return itsAddr;
}

/******************************************************************************
 Enabled

 *****************************************************************************/

inline JBoolean
CMBreakpoint::IsEnabled()
	const
{
	return itsEnabledFlag;
}

/******************************************************************************
 Action when breakpoint is hit

 *****************************************************************************/

inline CMBreakpoint::Action
CMBreakpoint::GetAction()
	const
{
	return itsAction;
}

/******************************************************************************
 Condition for when breakpoint is triggered

 *****************************************************************************/

inline JBoolean
CMBreakpoint::HasCondition()
	const
{
	return !itsCondition.IsEmpty();
}

inline JBoolean
CMBreakpoint::GetCondition
	(
	JString* condition
	)
	const
{
	*condition = itsCondition;
	return HasCondition();
}

/******************************************************************************
 Number of times breakpoint will be ignored

 *****************************************************************************/

inline JSize
CMBreakpoint::GetIgnoreCount()
	const
{
	return itsIgnoreCount;
}

#endif
