/******************************************************************************
 JArcTangent2.cpp

							The Arctangent w/ 2 arguments Class

	This class computes the arctangent to within (-180,180].

	BASE CLASS = JBinaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JArcTangent2.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcTangent2::JArcTangent2
	(
	JFunction* arg1,
	JFunction* arg2
	)
	:
	JBinaryFunction("arctan2", arg1, arg2)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcTangent2::~JArcTangent2()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcTangent2::JArcTangent2
	(
	const JArcTangent2& source
	)
	:
	JBinaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JArcTangent2::Copy()
	const
{
	auto* newFunction = jnew JArcTangent2(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JArcTangent2::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat y,x;
	if (!(GetArg1())->Evaluate(&y))
	{
		return false;
	}
	if (!(GetArg2())->Evaluate(&x))
	{
		return false;
	}
	jclear_errno();
	*result = atan2(y,x);
	return jerrno_is_clear();
}

bool
JArcTangent2::Evaluate
	(
	JComplex* result
	)
	const
{
	JFloat r;
	if (Evaluate(&r))	// provide full range in case of real numbers
	{
		*result = r;
		return true;
	}

	JComplex y,x;
	if (!(GetArg2())->Evaluate(&x) || (real(x) == 0.0 && imag(x) == 0.0))
	{
		return false;
	}
	if (!(GetArg1())->Evaluate(&y))
	{
		return false;
	}
	jclear_errno();
	*result = atan(y/x);
	return jerrno_is_clear();
}
