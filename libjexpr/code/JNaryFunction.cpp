/******************************************************************************
 JNaryFunction.cpp

							The N-ary Function Class

	This abstract class defines the requirements for all n argument Functions.

	BASE CLASS = JFunctionWithArgs

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JNaryFunction.h>
#include <jAssert.h>

const JSize kMaxReasonableArgCount = 10;

/******************************************************************************
 Constructor

 ******************************************************************************/

JNaryFunction::JNaryFunction
	(
	const JFnNameIndex	nameIndex,
	const JFunctionType	type
	)
	:
	JFunctionWithArgs(nameIndex, type)
{
	itsArgList = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll, kMaxReasonableArgCount);
	assert( itsArgList != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNaryFunction::~JNaryFunction()
{
	jdelete itsArgList;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JNaryFunction::JNaryFunction
	(
	const JNaryFunction& source
	)
	:
	JFunctionWithArgs(source)
{
	itsArgList = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll, kMaxReasonableArgCount);
	assert( itsArgList != NULL );

	const JSize argCount = (source.itsArgList)->GetElementCount();

	for (JIndex i=1; i<=argCount; i++)
		{
		JFunction* sourceArg = (source.itsArgList)->GetElement(i);
		JFunction* arg       = sourceArg->Copy();
		itsArgList->Append(arg);
		}
}

/******************************************************************************
 SameAs

	Derived classes must use SameAsAnyOrder, SameAsSameOrder,
	or provide their own code.

 ******************************************************************************/

JBoolean
JNaryFunction::SameAs
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
JNaryFunction::SameAsSameOrder
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunctionWithArgs::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JNaryFunction& theNaryFunction = (const JNaryFunction&) theFunction;

	const JSize count = GetArgCount();
	if (count != theNaryFunction.GetArgCount())
		{
		return kJFalse;
		}

	for (JIndex i=1; i<=count; i++)
		{
		const JFunction* itsArg = GetArg(i);
		const JFunction* theArg = theNaryFunction.GetArg(i);
		if (!itsArg->SameAs(*theArg))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 SameAsAnyOrder (protected)

	Returns kJTrue if the given function is identical to us.
	The arguments of one can be any permutation of the arguments of the other,
	but there must be a one-to-one correspondence between them.

 ******************************************************************************/

JBoolean
JNaryFunction::SameAsAnyOrder
	(
	const JFunction& theFunction
	)
	const
{
	if (!JFunctionWithArgs::SameAs(theFunction))
		{
		return kJFalse;
		}

	const JNaryFunction& theNaryFunction = (const JNaryFunction&) theFunction;

	const JSize count = GetArgCount();
	if (count != theNaryFunction.GetArgCount())
		{
		return kJFalse;
		}

	// Since the arguments can be in any order, we have to keep track
	// of which arguments have already been matched.

	JArray<JBoolean> usedFlags(count);
	{
	for (JIndex i=1; i<=count; i++)
		{
		usedFlags.InsertElementAtIndex(i, kJFalse);
		}
	}
	{
	for (JIndex i=1; i<=count; i++)
		{
		const JFunction* itsArg = GetArg(i);
		JBoolean matched = kJFalse;
		for (JIndex j=1; j<=count; j++)
			{
			if (!usedFlags.GetElement(j))
				{
				const JFunction* theArg = theNaryFunction.GetArg(j);
				if (itsArg->SameAs(*theArg))
					{
					matched = kJTrue;
					usedFlags.SetElement(j, kJTrue);
					break;
					}
				}
			}
		if (!matched)
			{
			return kJFalse;
			}
		}
	}

	return kJTrue;
}

/******************************************************************************
 GetArgCount

	Not inline because it is virtual

 ******************************************************************************/

JSize
JNaryFunction::GetArgCount()
	const
{
	return itsArgList->GetElementCount();
}

/******************************************************************************
 GetArg

	Not inline because they are virtual

 ******************************************************************************/

const JFunction*
JNaryFunction::GetArg
	(
	const JIndex index
	)
	const
{
	return itsArgList->GetElement(index);
}

JFunction*
JNaryFunction::GetArg
	(
	const JIndex index
	)
{
	return itsArgList->GetElement(index);
}

/******************************************************************************
 SetArg

 ******************************************************************************/

void
JNaryFunction::SetArg
	(
	const JIndex	index,
	JFunction*		arg
	)
{
	assert( arg != NULL );
	if (itsArgList->IndexValid(index))
		{
		itsArgList->SetElement(index, arg, JPtrArrayT::kDelete);
		}
	else
		{
		assert( index == itsArgList->GetElementCount()+1 );
		itsArgList->Append(arg);
		}
}

/******************************************************************************
 InsertArg

 ******************************************************************************/

void
JNaryFunction::InsertArg
	(
	const JIndex	index,
	JFunction*		arg
	)
{
	assert( arg != NULL );
	itsArgList->InsertAtIndex(index, arg);
}

/******************************************************************************
 PrependArg

 ******************************************************************************/

void
JNaryFunction::PrependArg
	(
	JFunction* arg
	)
{
	assert( arg != NULL );
	itsArgList->Prepend(arg);
}

/******************************************************************************
 AppendArg

 ******************************************************************************/

void
JNaryFunction::AppendArg
	(
	JFunction* arg
	)
{
	assert( arg != NULL );
	itsArgList->Append(arg);
}

/******************************************************************************
 DeleteArg

	This is allowed as long as there is at least one argument remaining.

 ******************************************************************************/

void
JNaryFunction::DeleteArg
	(
	const JIndex index
	)
{
	assert( itsArgList->GetElementCount() > 1 );
	itsArgList->DeleteElement(index);
}

JBoolean
JNaryFunction::DeleteArg
	(
	const JFunction* arg
	)
{
	assert( itsArgList->GetElementCount() > 1 );

	JIndex argIndex;
	const JBoolean found = itsArgList->Find(arg, &argIndex);
	if (found)
		{
		itsArgList->DeleteElement(argIndex);
		}
	return found;
}

/******************************************************************************
 Cast to JNaryFunction*

	Not inline because they are virtual

 ******************************************************************************/

JNaryFunction*
JNaryFunction::CastToJNaryFunction()
{
	return this;
}

const JNaryFunction*
JNaryFunction::CastToJNaryFunction()
	const
{
	return this;
}
