/******************************************************************************
 jAssert.cpp

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jAssert.h>
#include <jGlobals.h>

int
JAssert
	(
	const char*	expr,
	const char*	file,
	const int	line
	)
{
	JAssertBase* ah;
	if (JGetAssertHandler(&ah))
		{
		return ah->Assert(expr, file, line);
		}
	else
		{
		return JAssertBase::DefaultAssert(expr, file, line);
		}
}
