/******************************************************************************
 JBinaryOperator.cpp

							The Binary Operator Class

	This abstract class defines the requirements for all 2 argument operators.
	We provide a more appropriate default implementation of Print.

	BASE CLASS = JBinaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JBinaryOperator.h"
#include "jFunctionUtil.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBinaryOperator::JBinaryOperator
	(
	const JUtf8Byte*	name,
	JFunction*			arg1,
	JFunction*			arg2
	)
	:
	JBinaryFunction(name, arg1, arg2)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBinaryOperator::~JBinaryOperator()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBinaryOperator::JBinaryOperator
	(
	const JBinaryOperator& source
	)
	:
	JBinaryFunction(source)
{
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JBinaryOperator::Print
	(
	std::ostream& output
	)
	const
{
	PrintArg(output, GetArg1());
	GetName().Print(output);
	PrintArg(output, GetArg2());
}

/******************************************************************************
 PrintArg (protected)

 ******************************************************************************/

void
JBinaryOperator::PrintArg
	(
	std::ostream&		output,
	const JFunction*	arg
	)
	const
{
	if (ParenthesizeArgForPrint(*this, *arg))
		{
		output << '(';
		arg->Print(output);
		output << ')';
		}
	else
		{
		arg->Print(output);
		}
}
