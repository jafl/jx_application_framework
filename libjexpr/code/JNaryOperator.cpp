/******************************************************************************
 JNaryOperator.cpp

							The Nary Operator Class

	This abstract class defines the requirements for all 2 argument operators.
	We provide a more appropriate default implementation of Print.

	BASE CLASS = JNaryFunction

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JNaryOperator.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JNaryOperator::JNaryOperator
	(
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JNaryFunction(nameIndex, type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNaryOperator::~JNaryOperator()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JNaryOperator::JNaryOperator
	(
	const JNaryOperator& source
	)
	:
	JNaryFunction(source)
{
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JNaryOperator::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	return JNaryFunction::SameAsAnyOrder(theFunction);
}

/******************************************************************************
 Cast to JNaryOperator*

	Not inline because they are virtual

 ******************************************************************************/

JNaryOperator*
JNaryOperator::CastToJNaryOperator()
{
	return this;
}

const JNaryOperator*
JNaryOperator::CastToJNaryOperator()
	const
{
	return this;
}
