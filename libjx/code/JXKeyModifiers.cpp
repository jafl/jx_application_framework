/******************************************************************************
 JXKeyModifiers.cpp

	Class to represent the states of the modifiers keys that X defines.

	Copyright (C) 1996-2000 John Lindal.

 ******************************************************************************/

#include "JXKeyModifiers.h"
#include "JXDisplay.h"
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

	Returns true if the specified modifier key is available.

 ******************************************************************************/

bool
JXKeyModifiers::Available
	(
	const JIndex i
	)
	const
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	return MapKey(i) > 0;
};

/******************************************************************************
 GetState

	Return the state of the specified key.

 ******************************************************************************/

bool
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

bool
JXKeyModifiers::AllOff()
	const
{
	for (JIndex i=1; i<=kXModifierCount; i++)
		{
		if (itsState[i])
			{
			return false;
			}
		}
	return true;
}

/******************************************************************************
 SetState

	Set the state of the specified key.

 ******************************************************************************/

void
JXKeyModifiers::SetState
	(
	const JIndex	i,
	const bool	pushed
	)
{
	assert( 1 <= i && i <= kJXKeyModifierMapCount );
	itsState [ MapKey(i) ] = pushed;
	itsState [ 0         ] = false;	// invariant
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

	itsState[0] = false;

	for (JIndex i=1; i<=kXModifierCount; i++)
		{
		itsState[i] = (state & (1L << (i-1))) != 0;
		}
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JXKeyModifiers::Clear()
{
	for (JUnsignedOffset i=0; i<=kXModifierCount; i++)
		{
		itsState[i] = false;
		}
}

/******************************************************************************
 GetState (static)

	Get the state of the given key in the given bit vector.

 ******************************************************************************/

bool
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
		return (state & (1L << (j-1))) != 0;
		}
	else
		{
		return false;
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
	const bool		pushed
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
