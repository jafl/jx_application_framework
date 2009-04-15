/******************************************************************************
 JXKeyModifiers.cpp

	Class to represent the states of the modifiers keys that X defines.

	Copyright © 1996-2000 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXKeyModifiers.h>
#include <JXDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXKeyModifiers::JXKeyModifiers
	(
	const JXDisplay* display
	)
{
	itsMap = display->GetJXKeyModifierMapping();
	Clear();
}

JXKeyModifiers::JXKeyModifiers
	(
	const JXDisplay*	display,
	const unsigned int	state
	)
{
	SetState(display, state);
}

/******************************************************************************
 Available

	Returns kJTrue if the specified modifier key is available.

 ******************************************************************************/

JBoolean
JXKeyModifiers::Available
	(
	const JIndex i
	)
	const
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	return JI2B( MapKey(i) > 0 );
};

/******************************************************************************
 GetState

	Return the state of the specified key.

 ******************************************************************************/

JBoolean
JXKeyModifiers::GetState
	(
	const JIndex i
	)
	const
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	return itsState [ MapKey(i) ];
}

/******************************************************************************
 AllOff

 ******************************************************************************/

JBoolean
JXKeyModifiers::AllOff()
	const
{
	for (JIndex i=1; i<=kXModifierCount; i++)
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

	Set the state of the specified key.

 ******************************************************************************/

void
JXKeyModifiers::SetState
	(
	const JIndex	i,
	const JBoolean	pushed
	)
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	itsState [ MapKey(i) ] = pushed;
	itsState [ 0         ] = kJFalse;	// invariant
}

/******************************************************************************
 GetState

	Return the state as a bit vector.

 ******************************************************************************/

unsigned int
JXKeyModifiers::GetState()
	const
{
	unsigned int state = 0;
	for (JIndex i=1; i<=kXModifierCount; i++)
		{
		if (itsState[i])
			{
			state |= (1L << (i-1));
			}
		}
	return state;
}

/******************************************************************************
 SetState

	Set the state of all the keys.

 ******************************************************************************/

void
JXKeyModifiers::SetState
	(
	const JXDisplay*	display,
	const unsigned int	state
	)
{
	itsMap = display->GetJXKeyModifierMapping();

	itsState[0] = kJFalse;

	for (JIndex i=1; i<=kXModifierCount; i++)
		{
		itsState[i] = JI2B( (state & (1L << (i-1))) != 0 );
		}
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JXKeyModifiers::Clear()
{
	for (JIndex i=0; i<=kXModifierCount; i++)
		{
		itsState[i] = kJFalse;
		}
}

/******************************************************************************
 GetState (static)

	Get the state of the given key in the given bit vector.

 ******************************************************************************/

JBoolean
JXKeyModifiers::GetState
	(
	const JXDisplay*	display,
	const unsigned int	state,
	const JIndex		i
	)
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	const int* map = display->GetJXKeyModifierMapping();
	const int j    = map[i];
	if (j > 0)
		{
		return JI2B( (state & (1L << (j-1))) != 0 );
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetState (static)

	Set the state of the given key in the given bit vector.

 ******************************************************************************/

unsigned int
JXKeyModifiers::SetState
	(
	const JXDisplay*	display,
	const unsigned int	state,
	const JIndex		i,
	const JBoolean		pushed
	)
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	const int* map = display->GetJXKeyModifierMapping();
	const int j    = map[i];
	if (j == 0)
		{
		return state;
		}
	else if (pushed)
		{
		return (state | (1L << (j-1)));
		}
	else
		{
		return (state & ~(1L << (j-1)));
		}
}
