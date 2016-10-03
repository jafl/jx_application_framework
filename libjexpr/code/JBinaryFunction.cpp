/******************************************************************************
 JBinaryFunction.cpp

							The Binary Function Class

	This abstract class defines the requirements for all 2 argument Functions.

	BASE CLASS = JFunctionWithArgs

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JBinaryFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBinaryFunction::JBinaryFunction
	(
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JFunctionWithArgs(nameIndex, type)
{
	itsArg1 = NULL;
	itsArg2 = NULL;
}

JBinaryFunction::JBinaryFunction
	(
	JFunction*			arg1,
	JFunction*			arg2,
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JFunctionWithArgs(nameIndex, type)
{
	itsArg1 = arg1;
	itsArg2 = arg2;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBinaryFunction::~JBinaryFunction()
{
	delete itsArg1;
	delete itsArg2;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBinaryFunction::JBinaryFunction
	(
	const JBinaryFunction& source
	)
	:
	JFunctionWithArgs(source)
{
	itsArg1 = (source.itsArg1)->Copy();
	itsArg2 = (source.itsArg2)->Copy();
}

/******************************************************************************
 SameAs

	Derived classes must use SameAsEitherOrder, SameAsSameOrder,
	or provide their own code.

 ******************************************************************************/

JBoolean
JBinaryFunction::SameAs
	(
	const JFunction& theFunction
	)
	const
{
	assert( 0 );
	return kJFalse;
}

/******************************************************************************
 SameAsSameOrder (protected)

	Returns kJTrue if the given function is identical to us.
	The arguments in corresponding slots must be identical.

 ******************************************************************************/

JBoolean
JBinaryFunction::SameAsSameOrder
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunctionWithArgs::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JBinaryFunction& theBinaryFunction = (const JBinaryFunction&) theFunction;
	return JConvertToBoolean(
			itsArg1->SameAs(*(theBinaryFunction.itsArg1)) &&
			itsArg2->SameAs(*(theBinaryFunction.itsArg2)) );
}

/******************************************************************************
 SameAsEitherOrder (protected)

	Returns kJTrue if the given function is identical to us.
	The arguments can be in either order.

 ******************************************************************************/

JBoolean
JBinaryFunction::SameAsEitherOrder
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunctionWithArgs::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JBinaryFunction& theBinaryFunction = (const JBinaryFunction&) theFunction;
	if (itsArg1->SameAs(*(theBinaryFunction.itsArg1)))
		{
		return itsArg2->SameAs(*(theBinaryFunction.itsArg2));
		}
	if (itsArg1->SameAs(*(theBinaryFunction.itsArg2)))
		{
		return itsArg2->SameAs(*(theBinaryFunction.itsArg1));
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetArgCount

	This function is virtual so we can't inline it.

 ******************************************************************************/

JSize
JBinaryFunction::GetArgCount()
	const
{
	return 2;
}

/******************************************************************************
 GetArg

	This function is virtual so we can't inline it.

 ******************************************************************************/

const JFunction*
JBinaryFunction::GetArg
	(
	const JIndex index
	)
	const
{
	assert( index == 1 || index == 2);
	if (index == 1)
		{
		return itsArg1;
		}
	else	// index = 2
		{
		return itsArg2;
		}
}

JFunction*
JBinaryFunction::GetArg
	(
	const JIndex index
	)
{
	assert( index == 1 || index == 2);
	if (index == 1)
		{
		return itsArg1;
		}
	else	// index = 2
		{
		return itsArg2;
		}
}

/******************************************************************************
 SetArg

	This function is virtual so we can't inline it.

 ******************************************************************************/

void
JBinaryFunction::SetArg
	(
	const JIndex	index,
	JFunction*		arg
	)
{
	assert( index == 1 || index == 2);

	if (index == 1)
		{
		delete itsArg1;
		itsArg1 = arg;
		}
	else	// index = 2
		{
		delete itsArg2;
		itsArg2 = arg;
		}
}

/******************************************************************************
 Cast to JBinaryFunction*

	Not inline because they are virtual

 ******************************************************************************/

JBinaryFunction*
JBinaryFunction::CastToJBinaryFunction()
{
	return this;
}

const JBinaryFunction*
JBinaryFunction::CastToJBinaryFunction()
	const
{
	return this;
}
