/******************************************************************************
 GLVarList.cpp

							The GLVarList Class

	BASE CLASS = JVariableList

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLVarList.h"
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	file format:
		{
		N <name> <value>
		N <name>[array size] <values>
		}
		*

 ******************************************************************************/

GLVarList::GLVarList()
	:
	JVariableList()
{
	GVarListX();
}

GLVarList::GLVarList
	(
	const GLVarList& list
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

GLVarList::GLVarList
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
		JUtf8Byte type;
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
				JStringIterator iter(&name);
				const bool foundBracket = iter.Next("[");
				assert( foundBracket && !iter.AtEnd() );

				iter.BeginMatch();
				iter.Next("]");
				const JString sizeStr = iter.FinishMatch().GetString();
				iter.SkipPrev(2 + sizeStr.GetCharacterCount());
				iter.RemoveAllNext();
				iter.Invalidate();

				JFloat x;
				const bool isNumber = sizeStr.ConvertToFloat(&x);
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
			const JUtf8Character typeStr(type);
			const JUtf8Byte* map[] =
				{
				"name", typeStr.GetBytes()
				};
			const JString errorStr = JGetString("UnsupportedVariable::GLVarList", map, sizeof(map));
			JGetUserNotification()->ReportError(errorStr);
			JIgnoreUntil(input, '\n');
			}
		input >> std::ws;
		}
	JIgnoreUntil(input, '\n');
}

// private

void
GLVarList::GVarListX()
{
	itsNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNames != nullptr );

	itsValues = jnew JArray<JFloat>;
	assert( itsValues != nullptr );

	itsArrays = jnew JPtrArray<GNArray>(JPtrArrayT::kDeleteAll);
	assert( itsArrays != nullptr );

	InstallCollection(itsNames);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLVarList::~GLVarList()
{
	jdelete itsNames;
	jdelete itsValues;
	jdelete itsArrays;
}

/******************************************************************************
 AddVariable

 ******************************************************************************/

bool
GLVarList::AddVariable
	(
	const JString&	name,
	const JFloat	value
	)
{
	JIndex index;
	if (NameValid(name) && !ParseVariableName(name, &index))
		{
		JString* varName = jnew JString(name);
		assert( varName != nullptr );
		itsNames->Append(varName);
		itsValues->AppendElement(value);
		itsArrays->AppendElement(static_cast<GNArray*>(nullptr));
		return true;
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("NameUsed::GLVarList"));
		return false;
		}
}

/******************************************************************************
 RemoveVariable

 ******************************************************************************/

void
GLVarList::RemoveVariable
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

bool
GLVarList::AddArray
	(
	const JString&	name,
	const GNArray&	values
	)
{
	if (NameValid(name))
		{
		JString* varName = jnew JString(name);
		assert( varName != nullptr );
		itsNames->Append(varName);
		itsValues->AppendElement(0.0);
		GNArray* newArray = jnew GNArray(values);
		assert( newArray != nullptr );
		itsArrays->AppendElement(newArray);
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 SetValue

 ******************************************************************************/

bool
GLVarList::SetValue
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
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 SetNumericValue

 ******************************************************************************/

void
GLVarList::SetNumericValue
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
GLVarList::SetNumericValue
	(
	const JIndex 	variableIndex, 
	const JIndex 	elementIndex,
	const JComplex& value
	)
{
}

/******************************************************************************
 Virtual functions for GLVarList class

	Not inline because they are virtual

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
GLVarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	return *(itsNames->GetElement(index));
}

void
GLVarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
	itsNames->GetElement(index)->Split("_", &s, 2);
	if (s.GetElementCount() == 2)
		{
		*name = *s.GetElement(1);
		*subscript = *s.GetElement(2);
		}
	else
		{
		*name = *s.GetElement(1);
		subscript->Clear();
		}
}

/******************************************************************************
 SetVariableName (public)

 ******************************************************************************/

bool
GLVarList::SetVariableName
	(
	const JIndex 	varIndex, 
	const JString&	str
	)
{
	JIndex index;
	if (!NameValid(str))
		{
		return false;
		}
	else if (ParseVariableName(str, &index) && index != varIndex)
		{
		JGetUserNotification()->ReportError(JGetString("NameUsed::GLVarList"));
		return false;
		}
	else
		{
		JString* varName = itsNames->GetElement(varIndex);
		*varName = str;
		Broadcast(VarNameChanged(varIndex));
		return true;
		}
}

/******************************************************************************
 IsArray

 ******************************************************************************/

bool
GLVarList::IsArray
	(
	const JIndex index
	)
	const
{
	return itsArrays->GetElement(index) != nullptr;
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

bool
GLVarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return elementIndex == 1 ||
			(IsArray(variableIndex) &&
			 (itsArrays->GetElement(variableIndex))->IndexValid(elementIndex));
}

/******************************************************************************
 GetNumericValue

 ******************************************************************************/

bool
GLVarList::GetNumericValue
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
		return true;
		}
	else if (values != nullptr && values->IndexValid(elementIndex))
		{
		*value = values->GetElement(elementIndex);
		return true;
		}
	else
		{
		return false;
		}
}

bool
GLVarList::GetNumericValue
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
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 GetVariableCount

 ******************************************************************************/

JSize
GLVarList::GetVariableCount()
	const
{
	return itsValues->GetElementCount();
}
