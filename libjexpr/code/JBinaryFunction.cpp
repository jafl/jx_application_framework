/******************************************************************************
 JBinaryFunction.cpp

							The Binary Function Class

	This abstract class defines the requirements for all 2 argument Functions.

	BASE CLASS = JFunctionWithArgs

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JBinaryFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBinaryFunction::JBinaryFunction
	(
	const JUtf8Byte*	name,
	JFunction*			arg1,
	JFunction*			arg2
	)
	:
	JFunctionWithArgs(name)
{
	itsArg1 = arg1;
	itsArg1->SetParent(this);

	itsArg2 = arg2;
	itsArg2->SetParent(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBinaryFunction::~JBinaryFunction()
{
	jdelete itsArg1;
	jdelete itsArg2;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBinaryFunction::JBinaryFunction
	(
	const JBinaryFunction& source
	)
	:
	JFunctionWithArgs(source),
	itsArg1(nullptr),
	itsArg2(nullptr)
{
	if (source.itsArg1 != nullptr)
	{
		itsArg1 = (source.itsArg1)->Copy();
		itsArg1->SetParent(this);
	}

	if (source.itsArg2 != nullptr)
	{
		itsArg2 = (source.itsArg2)->Copy();
		itsArg2->SetParent(this);
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
		SetArg1(arg);
	}
	else	// index = 2
	{
		SetArg2(arg);
	}
}
