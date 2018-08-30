/******************************************************************************
 JNaryOperator.cpp

							The Nary Operator Class

	This abstract class defines the requirements for all N argument operators.

	BASE CLASS = JNaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JNaryOperator.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JNaryOperator::JNaryOperator
	(
	const JUtf8Byte*		name,
	JPtrArray<JFunction>*	argList
	)
	:
	JNaryFunction(name, argList)
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
