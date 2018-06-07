/******************************************************************************
 GVarList.cpp

							The GVarList Class

	BASE CLASS = JVariableList

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GVarList.h"

#include <JUserNotification.h>
#include <JString.h>

#include <jParseFunction.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <jGlobals.h>

#include <string.h>
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

GVarList::GVarList()
	:
	JVariableList()
{
	GVarListX();
}

GVarList::GVarList
	(
	const GVarList& list
	)
	:
	JVariableList()
{
	GVarListX();
	
	*itsValues	= *(list.itsValues);
	
	const JSize ncount	= list.itsNames->GetElementCount();
	for (JIndex i = 1; i <= ncount; i++)
		{
		JString* str	= jnew JString(*(list.itsNames->GetElement(i)));
		assert(str != nullptr);
		itsNames->Append(str);
		}

	const JSize acount	= list.itsArrays->GetElementCount();
	for (JIndex i = 1; i <= acount; i++)
		{
		JArray<JFloat>* array	= list.itsArrays->GetElement(i);
		if (array != nullptr)
			{
			array	= jnew JArray<JFloat>(*(array));
			assert(array != nullptr);
			}
		itsArrays->Append(array);
		}
}

GVarList::GVarList
	(
	std::istream& input
	)
	:
	JVariableList()
{
	GVarListX();

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
				AddVariable(name, value);
				}
			else
				{
				const JSize nameLen = name.GetLength();
				JIndex bracketIndex;
				const JBoolean foundBracket = name.LocateSubstring("[", &bracketIndex);
				assert( foundBracket && bracketIndex < nameLen-1 );
				const JString sizeStr = name.GetSubstring(bracketIndex+1, nameLen-1);
				name.RemoveSubstring(bracketIndex, nameLen);
				JFloat x;
				const JBoolean isNumber = sizeStr.ConvertToFloat(&x);
				assert( isNumber );
				const JSize arraySize = JRound(x);
				GNArray values(arraySize);
				for (JIndex i=1; i<= arraySize; i++)
					{
					JFloat value;
					input >> value;
					values.AppendElement(value);
					}
				AddArray(name, values);
				}
			}
		else
			{
			JString errorStr = "Unsupported variable type 'x'";
			errorStr.SetCharacter(errorStr.GetLength()-1, type);
			JGetUserNotification()->ReportError(errorStr);
			JIgnoreUntil(input, '\n');
			}
		input >> std::ws;
		}
	JIgnoreUntil(input, '\n');
}

// private

void
GVarList::GVarListX()
{
	itsNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNames != nullptr );

	itsValues = jnew JArray<JFloat>;
	assert( itsValues != nullptr );

	itsArrays = jnew JPtrArray<GNArray>(JPtrArrayT::kDeleteAll);
	assert( itsArrays != nullptr );

	InstallOrderedSet(itsNames);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GVarList::~GVarList()
{
	jdelete itsNames;
	jdelete itsValues;
	jdelete itsArrays;
}

/******************************************************************************
 AddVariable

 ******************************************************************************/

JBoolean
GVarList::AddVariable
	(
	const JCharacter*	name,
	const JFloat		value
	)
{
	JIndex index;
	if (JNameValid(name) && !ParseVariableName(name, strlen(name), &index))
		{
		JString* varName = jnew JString(name);
		assert( varName != nullptr );
		itsNames->Append(varName);
		itsValues->AppendElement(value);
		itsArrays->AppendElement(static_cast<GNArray*>(nullptr));
		return kJTrue;
		}
	else
		{
		JGetUserNotification()->ReportError("This name is already taken.");
		return kJFalse;
		}
}

/******************************************************************************
 RemoveVariable

 ******************************************************************************/

void
GVarList::RemoveVariable
	(
	const JIndex index
	)
{
	assert(index <= itsNames->GetElementCount());
	itsNames->DeleteElement(index);
	itsValues->RemoveElement(index);
	itsArrays->RemoveElement(index);
}


/******************************************************************************
 AddArray

 ******************************************************************************/

JBoolean
GVarList::AddArray
	(
	const JCharacter*	name,
	const GNArray&	values
	)
{
	if (JNameValid(name))
		{
		JString* varName = jnew JString(name);
		assert( varName != nullptr );
		itsNames->Append(varName);
		itsValues->AppendElement(0.0);
		GNArray* newArray = jnew GNArray(values);
		assert( newArray != nullptr );
		itsArrays->AppendElement(newArray);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetValue

 ******************************************************************************/

JBoolean
GVarList::SetValue
	(
	const JIndex	index,
	const JFloat	value
	)
{
	if (IsVariable(index))
		{
		const JFloat oldValue = itsValues->GetElement(index);
		if (value != oldValue)
			{
			itsValues->SetElement(index, value);
			Broadcast(JVariableList::VarValueChanged(index,1));
			}
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
GVarList::SetNumericValue
	(
	const JIndex 	variableIndex, 
	const JIndex 	elementIndex,
	const JFloat	value
	)
{
	if (IsArray(variableIndex))
		{
		GNArray* values = itsArrays->GetElement(variableIndex);
		const JFloat oldValue = values->GetElement(elementIndex);
		if (value != oldValue)
			{
			values->SetElement(elementIndex, value);
			Broadcast(JVariableList::VarValueChanged(variableIndex,elementIndex));
			}
		}
	else
		{
		SetValue(variableIndex, value);
		}
}

void
GVarList::SetNumericValue
	(
	const JIndex 	variableIndex, 
	const JIndex 	elementIndex,
	const JComplex& value
	)
{

}

/******************************************************************************
 Virtual functions for GVarList class

	Not inline because they are virtual

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
GVarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	return *(itsNames->GetElement(index));
}

void
GVarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	const JString* fullName = itsNames->GetElement(index);
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
 SetVariableName (public)

 ******************************************************************************/

JBoolean
GVarList::SetVariableName
	(
	const JIndex 	varIndex, 
	const JString&	str
	)
{
	JIndex index;
	if (!JNameValid(str))
		{
		return kJFalse;
		}
	else if (ParseVariableName(str, strlen(str), &index) && index != varIndex)
		{
		(JGetUserNotification())->ReportError("This variable name is already used.");
		return kJFalse;
		}
	else
		{
		JString* varName = itsNames->GetElement(varIndex);
		*varName = str;
		Broadcast(VarNameChanged(varIndex));
		return kJTrue;
		}
}


/******************************************************************************
 IsNumeric

 ******************************************************************************/

JBoolean
GVarList::IsNumeric
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
GVarList::IsDiscrete
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
GVarList::IsArray
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( itsArrays->GetElement(index) != nullptr );
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

JBoolean
GVarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return JConvertToBoolean(
			elementIndex == 1 ||
			(IsArray(variableIndex) &&
			 (itsArrays->GetElement(variableIndex))->IndexValid(elementIndex)));
}

/******************************************************************************
 ValueIsKnown

 ******************************************************************************/

JBoolean
GVarList::ValueIsKnown
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
GVarList::GetUnknownValueSymbol
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
GVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	GNArray* values = itsArrays->GetElement(variableIndex);
	if (values == nullptr && elementIndex == 1)
		{
		*value = itsValues->GetElement(variableIndex);
		return kJTrue;
		}
	else if (values != nullptr && values->IndexValid(elementIndex))
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
GVarList::GetNumericValue
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
 SetDiscreteValue

 ******************************************************************************/

void
GVarList::SetDiscreteValue
	(
	const JIndex 	variableIndex, 
	const JIndex 	elementIndex,
	const JIndex	valueIndex
	)
{

}

/******************************************************************************
 GetDiscreteValue

 ******************************************************************************/

JIndex
GVarList::GetDiscreteValue
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
GVarList::GetDiscreteValueCount
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
GVarList::GetDiscreteValueName
	(
	const JIndex variableIndex,
	const JIndex valueIndex
	)
	const
{
	return kUnknownValueSymbol;
}

/******************************************************************************
 GetVariableCount (public)

 ******************************************************************************/

JSize
GVarList::GetVariableCount()
	const
{
	return itsValues->GetElementCount();
}
