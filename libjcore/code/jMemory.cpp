/******************************************************************************
 jMemory.cpp

	Useful memory allocation routines

	Copyright © 1995 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jMemory.h>
#include <jAssert.h>

/******************************************************************************
 JCreateBuffer

	Create a buffer.  At input, bufferSize specifies how big the buffer ought
	to be.  At exit, bufferSize is the actual size of the buffer.  This might
	be smaller than what was requested if memory is running low.

 ******************************************************************************/

JCharacter*
JCreateBuffer
	(
	JSize* bufferSize
	)
{
	JCharacter* buffer = NULL;
	while (buffer == NULL)
		{
		buffer = new JCharacter[ *bufferSize ];
		if (buffer == NULL && *bufferSize > 10)
			{
			*bufferSize /= 10;
			}
		else if (buffer == NULL)
			{
			assert( 0 );	// if this happens, the system is really screwed
			}
		}
	return buffer;
}
