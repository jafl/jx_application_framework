/******************************************************************************
 JXButtonStates.cpp

	Class to represent the states of the mouse buttons that X defines.

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXButtonStates.h>
#include <jAssert.h>

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

JBoolean
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

JBoolean
JXButtonStates::AllOff()
	const
{
	for (JIndex i=0; i<kXButtonCount; i++)
		{
		if (itsState[i])
			{
			return kJFalse;
			}
		}
	return kJTrue;
}

/******************************************************************************
 SetState

	Set the state of the specified button.

 ******************************************************************************/

void
JXButtonStates::SetState
	(
	const JIndex	i,
	const JBoolean	pushed
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
	unsigned int state = 0;
	for (JIndex i=0; i<kXButtonCount; i++)
		{
		if (itsState[i])
			{
			state |= (1L << (i+8));		// use the same bits as X
			}
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
	for (JIndex i=0; i<kXButtonCount; i++)
		{
		itsState[i] = JConvertToBoolean( (state & (1L << (i+8))) != 0 );
		}
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JXButtonStates::Clear()
{
	for (JIndex i=0; i<kXButtonCount; i++)
		{
		itsState[i] = kJFalse;
		}
}

/******************************************************************************
 GetState (static)

	Get the state of the given key in the given bit vector.

 ******************************************************************************/

JBoolean
JXButtonStates::GetState
	(
	const unsigned int	state,
	const JIndex		i
	)
{
	assert( 0 < i && i <= kXButtonCount );
	return JConvertToBoolean( (state & (1L << (i+7))) != 0 );
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
	const JBoolean		pushed
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
