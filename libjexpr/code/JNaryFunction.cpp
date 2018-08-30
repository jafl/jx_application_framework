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
	const JUtf8Byte*		name,
	JPtrArray<JFunction>*	argList
	)
	:
	JFunctionWithArgs(name),
	itsArgList(argList)
{
	if (argList == nullptr)
		{
		itsArgList = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll, kMaxReasonableArgCount);
		assert( itsArgList != nullptr );
		}
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
	assert( itsArgList != nullptr );

	const JSize argCount = (source.itsArgList)->GetElementCount();

	for (JIndex i=1; i<=argCount; i++)
		{
		JFunction* sourceArg = (source.itsArgList)->GetElement(i);
		JFunction* arg       = sourceArg->Copy();
		itsArgList->Append(arg);
		}
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
	assert( arg != nullptr );
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
	assert( arg != nullptr );
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
	assert( arg != nullptr );
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
	assert( arg != nullptr );
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
