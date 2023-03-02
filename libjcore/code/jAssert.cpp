/******************************************************************************
 jAssert.cpp

	Copyright (C) 1996-2017 by John Lindal.

 ******************************************************************************/

#include "jAssert.h"
#include "jGlobals.h"

int
JAssert
	(
	const char*	expr,
	const char*	file,
	const int	line,
	const char*	message,
	const char*	function
	)
{
	JAssertBase* ah;
	if (JGetAssertHandler(&ah))
	{
		return ah->Assert(expr, file, line, message, function);
	}
	else
	{
		return JAssertBase::DefaultAssert(expr, file, line, message, function);
	}
}
