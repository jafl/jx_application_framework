/******************************************************************************
 JXButtonStates.cpp

	Class to represent the states of the mouse buttons that X defines.

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JXButtonStates.h"
#include <algorithm>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXButtonStates::JXButtonStates()
{
	Clear();
}

JXButtonStates::JXButtonStates
	(
	const unsigned int state
	)
{
	SetState(state);
}

/******************************************************************************
 GetState

	Return the state of the specified button.

 ******************************************************************************/

bool
JXButtonStates::GetState
	(
	const JIndex i
	)
	const
{
	assert( 0 < i && i <= kXButtonCount );
	return itsState[i-1];
}

/******************************************************************************
 AllOff

 ******************************************************************************/

bool
JXButtonStates::AllOff()
	const
{
	return std::all_of(std::begin(itsState), std::end(itsState),
						[] (const bool v) { return !v; });
}

/******************************************************************************
 SetState

	Set the state of the specified button.

 ******************************************************************************/

void
JXButtonStates::SetState
	(
	const JIndex	i,
	const bool	pushed
	)
{
	assert( 0 < i && i <= kXButtonCount );
	itsState[i-1] = pushed;
}

/******************************************************************************
 GetState

	Return the state as a bit vector.

 ******************************************************************************/

unsigned int
JXButtonStates::GetState()
	const
{
	unsigned int state = 0, i = 0;
	for (const auto b : itsState)
	{
		if (b)
		{
			state |= (1L << (i+8));		// use the same bits as X
		}
		i++;
	}
	return state;
}

/******************************************************************************
 SetState

	Set the state of all the keys.

 ******************************************************************************/

void
JXButtonStates::SetState
	(
	const unsigned int state
	)
{
	for (JUnsignedOffset i=0; i<kXButtonCount; i++)
	{
		itsState[i] = (state & (1L << (i+8))) != 0;
	}
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JXButtonStates::Clear()
{
	std::fill(std::begin(itsState), std::end(itsState), false);
}

/******************************************************************************
 GetState (static)

	Get the state of the given key in the given bit vector.

 ******************************************************************************/

bool
JXButtonStates::GetState
	(
	const unsigned int	state,
	const JIndex		i
	)
{
	assert( 0 < i && i <= kXButtonCount );
	return (state & (1L << (i+7))) != 0;
}

/******************************************************************************
 SetState (static)

	Set the state of the given button in the given bit vector.

 ******************************************************************************/

unsigned int
JXButtonStates::SetState
	(
	const unsigned int	state,
	const JIndex		i,
	const bool		pushed
	)
{
	if (pushed)
	{
		return (state | (1L << (i+7)));
	}
	else
	{
		return (state & ~(1L << (i+7)));
	}
}
