/******************************************************************************
 JConstantValue.cpp

							The Constant Value Class

	This class represents a constant numeric value.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JConstantValue.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JConstantValue::JConstantValue
	(
	const JFloat value
	)
{
	itsValue = value;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JConstantValue::~JConstantValue()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JConstantValue::JConstantValue
	(
	const JConstantValue& source
	)
	:
	JFunction(source)
{
	itsValue = source.itsValue;
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JConstantValue::Copy()
	const
{
	auto* newFunction = jnew JConstantValue(*this);
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

bool
JConstantValue::Evaluate
	(
	JFloat* result
	)
	const
{
	*result = itsValue;
	return true;
}

bool
JConstantValue::Evaluate
	(
	JComplex* result
	)
	const
{
	*result = itsValue;
	return true;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JConstantValue::Print
	(
	std::ostream& output
	)
	const
{
	JString s(itsValue);
	s.Print(output);

// It would be nice to use the stream, but they aren't smart enough
// about issues like lots of 9's on the end.
//	const int origPrecis = output.precision(JString::kDefSigDigitCount);
//	output << itsValue;
//	output.precision(origPrecis);
}
