/******************************************************************************
 JNaryFunction.cpp

							The N-ary Function Class

	This abstract class defines the requirements for all n argument Functions.

	BASE CLASS = JFunctionWithArgs

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "JNaryFunction.h"
#include <jx-af/jcore/jAssert.h>

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
	}
	else
	{
		for (auto* arg : *argList)
		{
			arg->SetParent(this);
		}
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

	const JSize argCount = source.itsArgList->GetItemCount();

	for (JIndex i=1; i<=argCount; i++)
	{
		JFunction* sourceArg = source.itsArgList->GetItem(i);
		JFunction* arg       = sourceArg->Copy();
		itsArgList->Append(arg);
		arg->SetParent(this);
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
	return itsArgList->GetItemCount();
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
	return itsArgList->GetItem(index);
}

JFunction*
JNaryFunction::GetArg
	(
	const JIndex index
	)
{
	return itsArgList->GetItem(index);
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
		itsArgList->SetItem(index, arg, JPtrArrayT::kDelete);
	}
	else
	{
		assert( index == itsArgList->GetItemCount()+1 );
		itsArgList->Append(arg);
	}

	arg->SetParent(this);
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
	arg->SetParent(this);
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
	arg->SetParent(this);
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
	arg->SetParent(this);
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
	assert( itsArgList->GetItemCount() > 1 );
	itsArgList->DeleteItem(index);
}

bool
JNaryFunction::DeleteArg
	(
	const JFunction* arg
	)
{
	assert( itsArgList->GetItemCount() > 1 );

	JIndex argIndex;
	const bool found = itsArgList->Find(arg, &argIndex);
	if (found)
	{
		itsArgList->DeleteItem(argIndex);
	}
	return found;
}
