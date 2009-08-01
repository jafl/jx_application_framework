/******************************************************************************
 jWebUtil.cpp

	System independent URL utilities.

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jWebUtil.h>
#include <string.h>
#include <jAssert.h>

/******************************************************************************
 JIsURL

	Renames the specified directory.

 ******************************************************************************/

JBoolean
JIsURL
	(
	const JCharacter* s
	)
{
	return JI2B(strstr(s, "://") != NULL);
}
