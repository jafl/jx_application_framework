/******************************************************************************
 TestVarList.cpp

							The TestVarList Class

	This class implements the simplest possible variable list for testing
	the parser.

	BASE CLASS = JVariableList

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "TestVarList.h"
#include <jParseFunction.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JString kUnknownValueSymbol = "?";

/******************************************************************************
 Constructor

	file format:
		{
		N <name> <value>
		N <name>[array size] <values>
		}
		*

 ******************************************************************************/

TestVarList::TestVarList()
	:
	JVariableList()
{
	TestVarListX();
}

TestVarList::TestVarList
	(
	std::istream& input
	)
	:
	JVariableList()
{
	TestVarListX();

	input >> std::ws;
	while (input.peek() != '*')
		{
		JCharacter type;
		input >> type >> std::ws;
		if (type == 'N')
			{
			JString name = JReadUntilws(input);
			if (name.GetLastCharacter() != ']')
				{
				JFloat value;
				input >> value;
				AddNumericVar(name, value);
				}
			else
				{
				const JSize nameLen = name.GetLength();
				JIndex bracketIndex;
				const JBoolean foundBracket = name.LocateSubstring("[", &bracketIndex);
				assert( foundBracket && bracketIndex < nameLen-1 );
				const JString sizeStr = name.GetSubstring(bracketIndex+1, nameLen-1);
				name.RemoveSubstring(bracketIndex, nameLen);
				JSize arraySize;
				const JBoolean isNumber = sizeStr.ConvertToUInt(&arraySize);
				assert( isNumber );
				TVLNArray values(arraySize);
				for (JIndex i=1; i<= arraySize; i++)
					{
					JFloat value;
					input >> value;
					values.AppendElement(value);
					}
				AddNumericArray(name, values);
				}
			}
		else
			{
			JString errorStr = "Unsupported variable type 'x'";
			errorStr.SetCharacter(errorStr.GetLength()-1, type);
			(JGetUserNotification())->ReportError(errorStr);
			JIgnoreLine(input);
			}
		input >> std::ws;
		}
	JIgnoreLine(input);
}

// private

void
TestVarList::TestVarListX()
{
	itsNumericNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNumericNames != NULL );

	itsNumericValues = jnew JArray<JFloat>;
	assert( itsNumericValues != NULL );

	itsNumericArrays = jnew JPtrArray<TVLNArray>(JPtrArrayT::kDeleteAll);
	assert( itsNumericArrays != NULL );

	InstallOrderedSet(itsNumericNames);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestVarList::~TestVarList()
{
	jdelete itsNumericNames;
	jdelete itsNumericValues;
	jdelete itsNumericArrays;
}

/******************************************************************************
 AddNumericVar

 ******************************************************************************/

JBoolean
TestVarList::AddNumericVar
	(
	const JCharacter*	name,
	const JFloat		value
	)
{
	JIndex index;
	if (JNameValid(name) && !ParseVariableName(name, strlen(name), &index))
		{
		itsNumericNames->Append(name);
		itsNumericValues->AppendElement(value);
		itsNumericArrays->AppendElement(static_cast<TVLNArray*>(NULL));
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 AddNumericArray

 ******************************************************************************/

JBoolean
TestVarList::AddNumericArray
	(
	const JCharacter*	name,
	const TVLNArray&	values
	)
{
	if (JNameValid(name))
		{
		itsNumericNames->Append(name);
		itsNumericValues->AppendElement(0.0);
		itsNumericArrays->Append(values);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Virtual functions for TestVarList class

	Not inline because they are virtual

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
TestVarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	return *(itsNumericNames->NthElement(index));
}

void
TestVarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	const JString* fullName = itsNumericNames->NthElement(index);
	const JSize fullLen     = fullName->GetLength();

	JIndex subStart;
	if (fullName->LocateSubstring("_", &subStart) &&
		1 < subStart && subStart < fullLen)
		{
		*name = fullName->GetSubstring(1, subStart-1);
		*subscript = fullName->GetSubstring(subStart+1, fullLen);
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
TestVarList::IsNumeric
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
TestVarList::IsDiscrete
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
TestVarList::IsArray
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( itsNumericArrays->NthElement(index) != NULL );
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

JBoolean
TestVarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return JConvertToBoolean(
			elementIndex == 1 ||
			(IsArray(variableIndex) &&
			 (itsNumericArrays->NthElement(variableIndex))->IndexValid(elementIndex)));
}

/******************************************************************************
 ValueIsKnown

 ******************************************************************************/

JBoolean
TestVarList::ValueIsKnown
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
TestVarList::GetUnknownValueSymbol
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
TestVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	TVLNArray* values = itsNumericArrays->NthElement(variableIndex);
	if (values == NULL && elementIndex == 1)
		{
		*value = itsNumericValues->GetElement(variableIndex);
		return kJTrue;
		}
	else if (values != NULL && values->IndexValid(elementIndex))
		{
		*value = values->GetElement(elementIndex);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
TestVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JComplex*		value
	)
	const
{
	JFloat x;
	if (GetNumericValue(variableIndex, elementIndex, &x))
		{
		*value = x;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetNumericValue

 ******************************************************************************/

void
TestVarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JFloat	value
	)
{
	assert_msg( 0, "TestVarList::SetNumericValue() is not supported" );
}

void
TestVarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JComplex&	value
	)
{
	assert_msg( 0, "TestVarList::SetNumericValue() is not supported" );
}

/******************************************************************************
 GetDiscreteValue

 ******************************************************************************/

JIndex
TestVarList::GetDiscreteValue
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
TestVarList::GetDiscreteValueCount
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
TestVarList::GetDiscreteValueName
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
TestVarList::SetDiscreteValue
	(
	const JIndex variableIndex,
	const JIndex elementIndex,
	const JIndex valueIndex
	)
{
	assert_msg( 0, "TestVarList::SetDiscreteValue() is not supported" );
}
