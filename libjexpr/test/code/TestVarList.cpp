/******************************************************************************
 TestVarList.cpp

	The simplest possible variable list for testing the parser.

	BASE CLASS = JVariableList

	Written by John Lindal.

 ******************************************************************************/

#include "TestVarList.h"
#include <JUserInputFunction.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jStreamUtil.h>
#include <jMath.h>
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
	JString name, sizeStr;
	while (input.peek() != '*')
		{
		JUtf8Character type;
		input >> type >> std::ws;
		if (type == 'N')
			{
			name = JUserInputFunction::ConvertToGreek(JReadUntilws(input));
			if (name.GetLastCharacter() != ']')
				{
				JFloat value;
				input >> value;

				itsNumericNames->Append(name);
				itsNumericValues->AppendElement(value);
				itsNumericArrays->AppendElement(static_cast<TVLNArray*>(nullptr));
				}
			else
				{
				JStringIterator iter(&name);
				bool found = iter.Next("[");
				assert( found && !iter.AtEnd() );

				iter.BeginMatch();
				found = iter.Next("]");
				assert( found && iter.AtEnd() );

				sizeStr = iter.FinishMatch().GetString();

				iter.Prev("[");
				iter.RemoveAllNext();
				iter.Invalidate();

				JSize arraySize;
				const bool isNumber = sizeStr.ConvertToUInt(&arraySize);
				assert( isNumber );

				TVLNArray values(arraySize);
				for (JIndex i=1; i<=arraySize; i++)
					{
					JFloat value;
					input >> value;
					values.AppendElement(value);
					}

				itsNumericNames->Append(name);
				itsNumericValues->AppendElement(0.0);
				itsNumericArrays->Append(values);
				}
			}
		else
			{
			std::cerr << "Unsupported variable type '" << type << '\'' << std::endl;
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
	assert( itsNumericNames != nullptr );

	itsNumericValues = jnew JArray<JFloat>;
	assert( itsNumericValues != nullptr );

	itsNumericArrays = jnew JPtrArray<TVLNArray>(JPtrArrayT::kDeleteAll);
	assert( itsNumericArrays != nullptr );

	InstallCollection(itsNumericNames);
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
	return *(itsNumericNames->GetElement(index));
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
	const JString* fullName = itsNumericNames->GetElement(index);

	JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
	fullName->Split("_", &s, 2);

	name->Clear();
	if (s.GetElementCount() == 2)
		{
		*name      = *s.GetElement(1);
		*subscript = *s.GetElement(2);
		}

	if (name->IsEmpty() || subscript->IsEmpty())
		{
		*name = *fullName;
		subscript->Clear();
		}
}

/******************************************************************************
 IsArray

 ******************************************************************************/

bool
TestVarList::IsArray
	(
	const JIndex index
	)
	const
{
	return itsNumericArrays->GetElement(index) != nullptr;
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

bool
TestVarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex elementIndex
	)
	const
{
	return elementIndex == 1 ||
			(IsArray(variableIndex) &&
			 (itsNumericArrays->GetElement(variableIndex))->IndexValid(elementIndex));
}

/******************************************************************************
 GetNumericValue

 ******************************************************************************/

bool
TestVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	TVLNArray* values = itsNumericArrays->GetElement(variableIndex);
	if (values == nullptr && elementIndex == 1)
		{
		*value = itsNumericValues->GetElement(variableIndex);
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
