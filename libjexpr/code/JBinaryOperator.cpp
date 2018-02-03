/******************************************************************************
 JBinaryOperator.cpp

							The Binary Operator Class

	This abstract class defines the requirements for all 2 argument operators.
	We provide a more appropriate default implementation of Print.

	BASE CLASS = JBinaryFunction

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JBinaryOperator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBinaryOperator::JBinaryOperator
	(
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JBinaryFunction(nameIndex, type)
{
}

JBinaryOperator::JBinaryOperator
	(
	JFunction*			arg1,
	JFunction*			arg2,
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JBinaryFunction(arg1, arg2, nameIndex, type)
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
	if (GetPrintDestination() == kMathematica)
		{
		output << GetMathematicaName();
		}
	else
		{
		output << GetName();
		}
	PrintArg(output, GetArg2());
}

/******************************************************************************
 PrintArg (protected)

 ******************************************************************************/

void
JBinaryOperator::PrintArg
	(
	std::ostream&			output,
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

/******************************************************************************
 Cast to JBinaryOperator*

	Not inline because they are virtual

 ******************************************************************************/

JBinaryOperator*
JBinaryOperator::CastToJBinaryOperator()
{
	return this;
}

const JBinaryOperator*
JBinaryOperator::CastToJBinaryOperator()
	const
{
	return this;
}
