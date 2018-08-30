/******************************************************************************
 JConstantValue.cpp

							The Constant Value Class

	This class represents a constant numeric value.

	BASE CLASS = JFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JConstantValue.h>
#include <JString.h>
#include <jAssert.h>

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
	JConstantValue* newFunction = jnew JConstantValue(*this);
	assert( newFunction != nullptr );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JConstantValue::Evaluate
	(
	JFloat* result
	)
	const
{
	*result = itsValue;
	return kJTrue;
}

JBoolean
JConstantValue::Evaluate
	(
	JComplex* result
	)
	const
{
	*result = itsValue;
	return kJTrue;
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
