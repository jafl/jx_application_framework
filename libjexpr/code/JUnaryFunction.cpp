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
	const JUtf8Byte*	name,
	JFunction*			arg
	)
	:
	JFunctionWithArgs(name),
	itsArg(arg)
{
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
	JFunctionWithArgs(source),
	itsArg(nullptr)
{
	if (source.itsArg != nullptr)
		{
		itsArg = (source.itsArg)->Copy();
		}
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
	SetArg(arg);
}
