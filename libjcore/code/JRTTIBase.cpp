/******************************************************************************
 JRTTIBase.cpp

	Base class for all objects that need RTTI.

	Each derived class must provide a unique string as its type.  The best way
	to insure uniqueness is to build the string "<class name>::<owner name>".
	To help avoid typos, define this string as a static class constant so
	clients can use this constant instead of the actual string.  If they
	mistype the constant, the compiler will complain.  If they mistype the
	string, it will be a very subtle bug.

	BASE CLASS = none

	Copyright © 1997 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JRTTIBase.h>
#include <string.h>
#include <jAssert.h>

/******************************************************************************
 Destructor

 ******************************************************************************/

JRTTIBase::~JRTTIBase()
{
}

/******************************************************************************
 Is

	Returns kJTrue if we are of the given type.

 ******************************************************************************/

JBoolean
JRTTIBase::Is
	(
	const JCharacter* type
	)
	const
{
	assert( type != NULL );	// make sure static constant was initialized
	return JI2B( type == itsType || strcmp(type, itsType) == 0 );
}
