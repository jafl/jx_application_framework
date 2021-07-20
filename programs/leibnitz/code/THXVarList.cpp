/******************************************************************************
 THXVarList.cpp

	BASE CLASS = JVariableList

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "THXVarList.h"
#include <JExprParser.h>
#include <JUserInputFunction.h>
#include <JConstantValue.h>
#include <JStringIterator.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JString kXName("x");
static const JString kYName("y");
static const JString kTName("t");

/******************************************************************************
 Constructor

 ******************************************************************************/

THXVarList::THXVarList()
	:
	JVariableList()
{
	THXVarListX();
}

THXVarList::THXVarList
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	:
	JVariableList()
{
	THXVarListX();

	JSize varCount;
	input >> varCount;

	JArray<JIndex> misfitIndexList;
	JPtrArray<JString> misfitFnList(JPtrArrayT::kDeleteAll);

	JGetUserNotification()->SetSilent(true);		// complain the second time

	JExprParser p(this);

	for (JIndex i=1; i<=varCount; i++)
		{
		auto* name = jnew JString;
		assert( name != nullptr );
		input >> *name;
		name->Set(JUserInputFunction::ConvertToGreek(*name));
		itsNames->Append(name);

		auto* fStr = jnew JString;
		assert( fStr != nullptr );
		input >> *fStr;
		JFunction* f;
		if (p.Parse(*fStr, &f))
			{
			jdelete fStr;
			itsFunctions->Append(f);
			}
		else
			{
			itsFunctions->Append(nullptr);
			misfitIndexList.AppendElement(i + kUserFnOffset);
			misfitFnList.Append(fStr);
			}
		}

	JGetUserNotification()->SetSilent(false);

	const JSize misfitCount = misfitIndexList.GetElementCount();
	for (JIndex i=1; i<=misfitCount; i++)
		{
		const JString* fStr = misfitFnList.GetElement(i);
		JFunction* f;
		if (p.Parse(*fStr, &f))
			{
			const JIndex j = misfitIndexList.GetElement(i);
			itsFunctions->SetElement(j, f, JPtrArrayT::kDelete);
			}
		}
}

// private

void
THXVarList::THXVarListX()
{
	itsNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNames != nullptr );

	itsFunctions = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
	assert( itsFunctions != nullptr );

	InstallCollection(itsNames);

	// variables for plotting

	itsNames->Append(kXName);

	itsXValue = jnew JConstantValue(0.0);
	assert( itsXValue != nullptr );
	itsFunctions->Append(itsXValue);

	itsNames->Append(kYName);

	itsYValue = jnew JConstantValue(0.0);
	assert( itsYValue != nullptr );
	itsFunctions->Append(itsYValue);

	itsNames->Append(kTName);

	itsTValue = jnew JConstantValue(0.0);
	assert( itsTValue != nullptr );
	itsFunctions->Append(itsTValue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXVarList::~THXVarList()
{
	jdelete itsNames;
	jdelete itsFunctions;
}

/******************************************************************************
 NewFunction

 ******************************************************************************/

JIndex
THXVarList::NewFunction()
{
	auto* name = jnew JString;
	assert( name != nullptr );

	JUInt64 i = 1;
	JIndex j;
	do
		{
		*name = JGetString("NewVarName::THXVarList");
		if (i > 1)
			{
			*name += JString(i);
			}
		i++;
		}
		while (ParseVariableName(*name, &j));

	itsNames->Append(name);

	JFunction* f = jnew JConstantValue(0.0);
	assert( f != nullptr );
	itsFunctions->Append(f);

	const JIndex varIndex = GetElementCount();
	Broadcast(VarInserted(varIndex));
	return varIndex;
}

/******************************************************************************
 RemoveFunction

 ******************************************************************************/

void
THXVarList::RemoveFunction
	(
	const JIndex index
	)
{
	assert( index > kUserFnOffset );

	itsNames->DeleteElement(index);
	itsFunctions->DeleteElement(index);
	Broadcast(VarRemoved(index));
}

/******************************************************************************
 SetVariableName

 ******************************************************************************/

bool
THXVarList::SetVariableName
	(
	const JIndex	varIndex,
	const JString&	name
	)
{
	assert( varIndex > kUserFnOffset );

	JIndex index;
	if (!JVariableList::NameValid(name))
		{
		return false;
		}
	else if (ParseVariableName(name, &index) && index != varIndex)
		{
		JGetUserNotification()->ReportError(JGetString("NameUsed::THXVarList"));
		return false;
		}
	else
		{
		JString* varName = itsNames->GetElement(varIndex);
		*varName = name;
		Broadcast(VarNameChanged(varIndex));
		return true;
		}
}

/******************************************************************************
 SetFunction

 ******************************************************************************/

bool
THXVarList::SetFunction
	(
	const JIndex	index,
	const JString&	expr
	)
{
	assert( index > kUserFnOffset );

	JExprParser p(this);

	JFunction* f;
	if (p.Parse(expr, &f))
		{
		itsFunctions->SetElement(index, f, JPtrArrayT::kDelete);
		Broadcast(VarValueChanged(index,1));
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 Virtual functions for THXVarList class

	Not inline because they are virtual

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
THXVarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	return *(itsNames->GetElement(index));
}

void
THXVarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	const JString* fullName = itsNames->GetElement(index);
	if (fullName->GetCharacterCount() > 1)
		{
		*name      = fullName->GetFirstCharacter();
		*subscript = *fullName;

		JStringIterator iter(subscript);
		iter.RemoveNext(1);
		}
	else
		{
		*name = *fullName;
		subscript->Clear();
		}
}

/******************************************************************************
 IsArray

 ******************************************************************************/

bool
THXVarList::IsArray
	(
	const JIndex index
	)
	const
{
	return false;
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

bool
THXVarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return elementIndex == 1;
}

/******************************************************************************
 GetNumericValue

 ******************************************************************************/

bool
THXVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	const JFunction* f = itsFunctions->GetElement(variableIndex);
	if (elementIndex == 1)
		{
		if (IsOnEvalStack(variableIndex))
			{
			*value = 0.0;
			return false;
			}
		PushOnEvalStack(variableIndex);
		const bool ok = f->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
		}
	else
		{
		*value = 0.0;
		return false;
		}
}

bool
THXVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JComplex*		value
	)
	const
{
	const JFunction* f = itsFunctions->GetElement(variableIndex);
	if (elementIndex == 1)
		{
		if (IsOnEvalStack(variableIndex))
			{
			*value = 0.0;
			return false;
			}
		PushOnEvalStack(variableIndex);
		const bool ok = f->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
		}
	else
		{
		*value = 0.0;
		return false;
		}
}

/******************************************************************************
 SetNumericValue

 ******************************************************************************/

void
THXVarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JFloat	value
	)
{
	assert( elementIndex == 1 );

	if (variableIndex == kXIndex)
		{
		itsXValue->SetValue(value);
		}
	else if (variableIndex == kYIndex)
		{
		itsYValue->SetValue(value);
		}
	else if (variableIndex == kTIndex)
		{
		itsTValue->SetValue(value);
		}
	else
		{
		assert_msg( 0, "THXVarList::SetNumericValue() called for a function" );
		}
}

void
THXVarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JComplex&	value
	)
{
	assert_msg( 0, "THXVarList does not store complex values" );
}

/******************************************************************************
 Global functions for THXVarList class

 ******************************************************************************/

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::ostream&
operator<<
	(
	std::ostream&			output,
	const THXVarList&	varList
	)
{
	const JSize varCount = (varList.itsNames)->GetElementCount();
	output << varCount - THXVarList::kUserFnOffset;

	for (JIndex i = 1+THXVarList::kUserFnOffset; i<=varCount; i++)
		{
		const JString* name = (varList.itsNames)->GetElement(i);
		output << ' ' << *name;

		const JFunction* f = (varList.itsFunctions)->GetElement(i);
		output << ' ' << f->Print();
		}

	// allow chaining

	return output;
}
