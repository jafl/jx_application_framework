/******************************************************************************
 jASCIIConstants.h

	Defines useful ASCII constants

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jASCIIConstants
#define _H_jASCIIConstants

#include <jTypes.h>

const int kJMinASCIICharacter = 0;
const int kJMaxASCIICharacter = 127;

const JUtf8Byte kJDeleteKey   = '\b';
const JUtf8Byte kJTabKey      = '\t';
const JUtf8Byte kJLineFeedKey = '\n';
const JUtf8Byte kJNewlineKey  = '\n';
const JUtf8Byte kJFormFeedKey = '\f';
const JUtf8Byte kJReturnKey   = '\r';
const JUtf8Byte kJEscapeKey   = 27;

const JUtf8Byte kJLeftArrow  = 28;
const JUtf8Byte kJRightArrow = 29;
const JUtf8Byte kJUpArrow    = 30;
const JUtf8Byte kJDownArrow  = 31;

const JUtf8Byte kJForwardDeleteKey = 127;

inline JBoolean
JIsASCII
	(
	const int c
	)
{
	return JConvertToBoolean( kJMinASCIICharacter <= c && c <= kJMaxASCIICharacter );
}

#endif
