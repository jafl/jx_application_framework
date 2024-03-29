/******************************************************************************
 jTime.cpp

	Routines to augment time.c

	Copyright (C) 1995 John Lindal.

 ******************************************************************************/

#include <jTime.h>
#include <windows.h>
#include <jAssert.h>

/******************************************************************************
 JWait

	Waits for the specified number of seconds.

 ******************************************************************************/

void
JWait
	(
	const double delta
	)
{
	Sleep(delta * 1000);
}
