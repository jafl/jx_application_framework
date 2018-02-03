/******************************************************************************
 JRTTIBase.cpp

	Base class for all objects that need RTTI.

	Each derived class must provide a unique string as its type.  The best
	way to ensure uniqueness is to build the string "<class name>::<owner
	name>".  To help avoid typos, define this string as a static class
	constant so clients can use this constant instead of the actual string.
	If they mistype the constant, the compiler will complain.  If they
	mistype the string, it will be a very subtle bug.  In addition, using
	the constant makes Is() much faster.

	This was built long before compiler support for RTTI support was
	stable.  We could replace it with std::type_info, but since Is()
	already does a pointer comparison, there is no speed benefit, and the
	reduction in static data would be negligible.

	BASE CLASS = none

	Copyright (C) 1997 by John Lindal.

 *****************************************************************************/

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
	const JUtf8Byte* type
	)
	const
{
	assert( type != NULL );
	return JI2B( type == itsType || strcmp(type, itsType) == 0 );
}
