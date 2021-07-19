/******************************************************************************
 JNamedConstant.cpp

							The Named Constant Class

	This class represents a named constant numeric value.

	Since we only store the index of the constant, this can't be used
	for anything other than constants defined at compile time.
	User-defined values must be represented by something else.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JNamedConstant.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <jMath.h>
#include <JString.h>
#include <jAssert.h>

static const JFloat kNamedConstValues[] =
{
	kJPi, kJE, 0.0
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JNamedConstant::JNamedConstant
	(
	const JNamedConstIndex nameIndex
	)
{
	assert( 1 <= nameIndex && nameIndex <= kJNamedConstCount );
	itsNameIndex = nameIndex;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNamedConstant::~JNamedConstant()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JNamedConstant::JNamedConstant
	(
	const JNamedConstant& source
	)
	:
	JFunction(source)
{
	itsNameIndex = source.itsNameIndex;
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JNamedConstant::Copy()
	const
{
	auto* newFunction = jnew JNamedConstant(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JNamedConstant::Evaluate
	(
	JFloat* result
	)
	const
{
	if (itsNameIndex == kIJNamedConstIndex)		// i is not a real number
		{
		*result = 0.0;
		return false;
		}
	else
		{
		*result = kNamedConstValues[ itsNameIndex-1 ];
		return true;
		}
}

bool
JNamedConstant::Evaluate
	(
	JComplex* result
	)
	const
{
	if (itsNameIndex == kIJNamedConstIndex)		// i is not a real number
		{
		*result = JComplex(0.0, 1.0);
		}
	else
		{
		*result = kNamedConstValues[ itsNameIndex-1 ];
		}

	return true;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JNamedConstant::Print
	(
	std::ostream& output
	)
	const
{
	if (itsNameIndex == kIJNamedConstIndex)
		{
		output << JGetCurrentImagString();
		}
	else if (itsNameIndex == kPiJNamedConstIndex)
		{
		output << "\xCF\x80";
		}
	else
		{
		assert( itsNameIndex == kEJNamedConstIndex );
		output << 'e';
		}
}

/******************************************************************************
 IsNamedConstant (static)

 ******************************************************************************/

bool
JNamedConstant::IsNamedConstant
	(
	const JString& name
	)
{
	return name == JGetCurrentImagString() ||
				 name == "\xCF\x80"              ||
				 name == "e";
}
