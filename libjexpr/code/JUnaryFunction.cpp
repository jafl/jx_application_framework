/******************************************************************************
 JUnaryFunction.cpp

							The Unary JFunction Class

	This abstract class defines the requirements for all 1 argument Functions.

	BASE CLASS = JFunctionWithArgs

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JUnaryFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JUnaryFunction::JUnaryFunction
	(
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JFunctionWithArgs(nameIndex, type)
{
	itsArg = nullptr;
}

JUnaryFunction::JUnaryFunction
	(
	JFunction*			arg,
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JFunctionWithArgs(nameIndex, type)
{
	itsArg = arg;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUnaryFunction::~JUnaryFunction()
{
	jdelete itsArg;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JUnaryFunction::JUnaryFunction
	(
	const JUnaryFunction& source
	)
	:
	JFunctionWithArgs(source)
{
	itsArg = (source.itsArg)->Copy();
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JUnaryFunction::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunctionWithArgs::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JUnaryFunction& theUnaryFunction = (const JUnaryFunction&) theFunction;
	return itsArg->SameAs(*(theUnaryFunction.itsArg));
}

/******************************************************************************
 GetArgCount

	This function is virtual so we can't inline it.

 ******************************************************************************/

JSize
JUnaryFunction::GetArgCount()
	const
{
	return 1;
}

/******************************************************************************
 GetArg

	This function is virtual so we can't inline it.

 ******************************************************************************/

const JFunction*
JUnaryFunction::GetArg
	(
	const JIndex index
	)
	const
{
	assert( index == 1);
	return itsArg;
}

JFunction*
JUnaryFunction::GetArg
	(
	const JIndex index
	)
{
	assert( index == 1);
	return itsArg;
}

/******************************************************************************
 SetArg

	This function is virtual so we can't inline it.

 ******************************************************************************/

void
JUnaryFunction::SetArg
	(
	const JIndex	index,
	JFunction*		arg
	)
{
	assert( index == 1);
	jdelete itsArg;
	itsArg = arg;
}

/******************************************************************************
 Cast to JUnaryFunction*

	Not inline because they are virtual

 ******************************************************************************/

JUnaryFunction*
JUnaryFunction::CastToJUnaryFunction()
{
	return this;
}

const JUnaryFunction*
JUnaryFunction::CastToJUnaryFunction()
	const
{
	return this;
}
