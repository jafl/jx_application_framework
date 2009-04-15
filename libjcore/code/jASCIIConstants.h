/******************************************************************************
 jASCIIConstants.h

	Defines useful ASCII constants

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jASCIIConstants
#define _H_jASCIIConstants

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <ctype.h>	// for convenience

const int kJMinASCIICharacter = 0;
const int kJMaxASCIICharacter = 127;

const JCharacter kJDeleteKey   = '\b';
const JCharacter kJTabKey      = '\t';
const JCharacter kJLineFeedKey = '\n';
const JCharacter kJNewlineKey  = '\n';
const JCharacter kJFormFeedKey = '\f';
const JCharacter kJReturnKey   = '\r';
const JCharacter kJEscapeKey   = 27;

const JCharacter kJLeftArrow  = 28;
const JCharacter kJRightArrow = 29;
const JCharacter kJUpArrow    = 30;
const JCharacter kJDownArrow  = 31;

const JCharacter kJForwardDeleteKey = 127;

inline JBoolean
JIsASCII
	(
	const int c
	)
{
	return JConvertToBoolean( kJMinASCIICharacter <= c && c <= kJMaxASCIICharacter );
}

#endif
