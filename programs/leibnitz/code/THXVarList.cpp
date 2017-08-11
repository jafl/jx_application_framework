/******************************************************************************
 THXVarList.cpp

	BASE CLASS = JVariableList

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THXVarList.h"
#include <jParseFunction.h>
#include <JConstantValue.h>
#include <jParserData.h>
#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kXName = "x";
static const JCharacter* kYName = "y";
static const JCharacter* kTName = "t";

static const JCharacter* kNewVarName     = "new";
static const JString kUnknownValueSymbol = "?";

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
JIndex i;

	THXVarListX();

	JSize varCount;
	input >> varCount;

	JArray<JIndex> misfitIndexList;
	JPtrArray<JString> misfitFnList(JPtrArrayT::kDeleteAll);

	(JGetUserNotification())->SetSilent(kJTrue);		// complain the second time

	for (i=1; i<=varCount; i++)
		{
		JString* name = jnew JString;
		assert( name != NULL );
		input >> *name;
		itsNames->Append(name);

		JString* fStr = jnew JString;
		assert( fStr != NULL );
		input >> *fStr;
		JFunction* f;
		if (JParseFunction(*fStr, this, &f))
			{
			jdelete fStr;
			itsFunctions->Append(f);
			}
		else
			{
			itsFunctions->Append(NULL);
			misfitIndexList.AppendElement(i + kUserFnOffset);
			misfitFnList.Append(fStr);
			}
		}

	(JGetUserNotification())->SetSilent(kJFalse);

	const JSize misfitCount = misfitIndexList.GetElementCount();
	for (i=1; i<=misfitCount; i++)
		{
		const JString* fStr = misfitFnList.NthElement(i);
		JFunction* f;
		if (JParseFunction(*fStr, this, &f))
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
	assert( itsNames != NULL );

	itsFunctions = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
	assert( itsFunctions != NULL );

	InstallOrderedSet(itsNames);

	// variables for plotting

	itsNames->Append(kXName);

	itsXValue = jnew JConstantValue(0.0);
	assert( itsXValue != NULL );
	itsFunctions->Append(itsXValue);

	itsNames->Append(kYName);

	itsYValue = jnew JConstantValue(0.0);
	assert( itsYValue != NULL );
	itsFunctions->Append(itsYValue);

	itsNames->Append(kTName);

	itsTValue = jnew JConstantValue(0.0);
	assert( itsTValue != NULL );
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
	JString* name = jnew JString;
	assert( name != NULL );

	JIndex i = 1,j;
	do
		{
		*name = kNewVarName;
		if (i > 1)
			{
			*name += JString(i);
			}
		i++;
		}
		while (ParseVariableName(*name, name->GetLength(), &j));

	itsNames->Append(name);

	JFunction* f = jnew JConstantValue(0.0);
	assert( f != NULL );
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

JBoolean
THXVarList::SetVariableName
	(
	const JIndex		varIndex,
	const JCharacter*	name
	)
{
	assert( varIndex > kUserFnOffset );

	JIndex index;
	if (!JNameValid(name))
		{
		return kJFalse;
		}
	else if (ParseVariableName(name, strlen(name), &index) && index != varIndex)
		{
		(JGetUserNotification())->ReportError("This variable name is already used.");
		return kJFalse;
		}
	else
		{
		JString* varName = itsNames->NthElement(varIndex);
		*varName = name;
		Broadcast(VarNameChanged(varIndex));
		return kJTrue;
		}
}

/******************************************************************************
 SetFunction

 ******************************************************************************/

JBoolean
THXVarList::SetFunction
	(
	const JIndex		index,
	const JCharacter*	expr
	)
{
	assert( index > kUserFnOffset );

	JFunction* f;
	if (JParseFunction(expr, this, &f))
		{
		itsFunctions->SetElement(index, f, JPtrArrayT::kDelete);
		Broadcast(VarValueChanged(index,1));
		return kJTrue;
		}
	else
		{
		return kJFalse;
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
	return *(itsNames->NthElement(index));
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
	const JString* fullName    = itsNames->NthElement(index);
	const JCharacter firstChar = fullName->GetFirstCharacter();
	const JSize fullLen        = fullName->GetLength();

	const JCharacter greekPrefixChar = JPGetGreekCharPrefixChar();
	if (firstChar == greekPrefixChar && fullLen > 2)
		{
		*name      = fullName->GetSubstring(1,2);
		*subscript = fullName->GetSubstring(3, fullLen);
		}
	else if (firstChar == greekPrefixChar)
		{
		*name = *fullName;
		subscript->Clear();
		}
	else if (fullLen > 1)
		{
		*name      = fullName->GetSubstring(1,1);
		*subscript = fullName->GetSubstring(2, fullLen);
		}
	else
		{
		*name = *fullName;
		subscript->Clear();
		}
}

/******************************************************************************
 IsNumeric

 ******************************************************************************/

JBoolean
THXVarList::IsNumeric
	(
	const JIndex index
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 IsDiscrete

 ******************************************************************************/

JBoolean
THXVarList::IsDiscrete
	(
	const JIndex index
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 IsArray

 ******************************************************************************/

JBoolean
THXVarList::IsArray
	(
	const JIndex index
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

JBoolean
THXVarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return JConvertToBoolean( elementIndex == 1 );
}

/******************************************************************************
 ValueIsKnown

 ******************************************************************************/

JBoolean
THXVarList::ValueIsKnown
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 GetUnknownValueSymbol

 ******************************************************************************/

const JString&
THXVarList::GetUnknownValueSymbol
	(
	const JIndex index
	)
	const
{
	return kUnknownValueSymbol;
}

/******************************************************************************
 GetNumericValue

 ******************************************************************************/

JBoolean
THXVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	const JFunction* f = itsFunctions->NthElement(variableIndex);
	if (elementIndex == 1)
		{
		if (IsOnEvalStack(variableIndex))
			{
			*value = 0.0;
			return kJFalse;
			}
		PushOnEvalStack(variableIndex);
		const JBoolean ok = f->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
		}
	else
		{
		*value = 0.0;
		return kJFalse;
		}
}

JBoolean
THXVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JComplex*		value
	)
	const
{
	const JFunction* f = itsFunctions->NthElement(variableIndex);
	if (elementIndex == 1)
		{
		if (IsOnEvalStack(variableIndex))
			{
			*value = 0.0;
			return kJFalse;
			}
		PushOnEvalStack(variableIndex);
		const JBoolean ok = f->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
		}
	else
		{
		*value = 0.0;
		return kJFalse;
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
 GetDiscreteValue

 ******************************************************************************/

JIndex
THXVarList::GetDiscreteValue
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return 1;
}

/******************************************************************************
 GetDiscreteValueCount

 ******************************************************************************/

JSize
THXVarList::GetDiscreteValueCount
	(
	const JIndex index
	)
	const
{
	return 0;
}

/******************************************************************************
 GetDiscreteValueName

 ******************************************************************************/

const JString&
THXVarList::GetDiscreteValueName
	(
	const JIndex variableIndex,
	const JIndex valueIndex
	)
	const
{
	return kUnknownValueSymbol;
}

/******************************************************************************
 SetDiscreteValue

 ******************************************************************************/

void
THXVarList::SetDiscreteValue
	(
	const JIndex variableIndex,
	const JIndex elementIndex,
	const JIndex valueIndex
	)
{
	assert_msg( 0, "THXVarList has no discrete values" );
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
		const JString* name = (varList.itsNames)->NthElement(i);
		output << ' ' << *name;

		const JFunction* f = (varList.itsFunctions)->NthElement(i);
		output << ' ' << f->Print();
		}

	// allow chaining

	return output;
}
