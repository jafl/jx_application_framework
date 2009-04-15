/******************************************************************************
 SCCircuitVarList.cpp

	BASE CLASS = JVariableList

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCCircuitVarList.h"
#include <jParseFunction.h>
#include <JFunction.h>
#include <jParserData.h>
#include <JUserNotification.h>
#include <JString.h>
#include <JPtrArray.h>
#include <jGlobals.h>
#include <jAssert.h>

static const JString kUnknownValueSymbol = "?";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCCircuitVarList::SCCircuitVarList()
	:
	JVariableList()
{
	SCCircuitVarListX();

	// we always need the frequency variables f,w,s

	JBoolean ok = AddVariable("f", 0.0, kJTrue);
	assert( ok );

	JFunction* f;
	ok = JParseFunction("2*pi*f", this, &f);
	assert( ok );
	ok = AddFunction("`w", *f, kJFalse);
	assert( ok );
	delete f;

	ok = JParseFunction("j*`w", this, &f);
	assert( ok );
	ok = AddFunction("s", *f, kJFalse);
	assert( ok );
	delete f;
}

SCCircuitVarList::SCCircuitVarList
	(
	istream&			input,
	const JFileVersion	vers
	)
	:
	JVariableList()
{
JIndex i;

	SCCircuitVarListX();

	JSize varCount;
	input >> varCount;

	JArray<JIndex> misfitIndexList;
	JPtrArray<JString> misfitFnList(JPtrArrayT::kDeleteAll);
	for (i=1; i<=varCount; i++)
		{
		VarInfo info;

		info.name = new JString;
		assert( info.name != NULL );
		input >> *(info.name);

		input >> info.value;

		JBoolean isFunction;
		input >> isFunction;
		if (isFunction)
			{
			JString* fStr = new JString;
			assert( fStr != NULL );
			input >> *fStr;
			(JGetUserNotification())->SetSilent(kJTrue);	// complain the second time
			if (JParseFunction(*fStr, this, &(info.f)))
				{
				delete fStr;
				}
			else
				{
				info.f = NULL;
				misfitIndexList.AppendElement(i);
				misfitFnList.Append(fStr);
				}
			(JGetUserNotification())->SetSilent(kJFalse);
			}

		input >> info.visible;

		itsVars->AppendElement(info);
		}

	const JSize misfitCount = misfitIndexList.GetElementCount();
	for (i=1; i<=misfitCount; i++)
		{
		const JIndex j      = misfitIndexList.GetElement(i);
		VarInfo info        = itsVars->GetElement(j);
		const JString* fStr = misfitFnList.NthElement(i);
		if (JParseFunction(*fStr, this, &(info.f)))
			{
			itsVars->SetElement(j, info);
			}
		}
}

// private

void
SCCircuitVarList::SCCircuitVarListX()
{
	itsVars = new JArray<VarInfo>;
	assert( itsVars != NULL );

	InstallOrderedSet(itsVars);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCCircuitVarList::~SCCircuitVarList()
{
	const JSize count = itsVars->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		VarInfo info = itsVars->GetElement(i);
		delete (info.name);
		delete (info.f);
		}
	delete itsVars;
}

/******************************************************************************
 AddVariable

 ******************************************************************************/

JBoolean
SCCircuitVarList::AddVariable
	(
	const JCharacter*	name,
	const JFloat		value,
	const JBoolean		visible
	)
{
	JIndex index;
	if (!JNameValid(name))
		{
		return kJFalse;
		}
	else if (ParseVariableName(name, strlen(name), &index))
		{
		(JGetUserNotification())->ReportError("This variable name is already used.");
		return kJFalse;
		}
	else
		{
		VarInfo info;

		info.name = new JString(name);
		assert( info.name != NULL );

		info.value   = value;
		info.visible = visible;

		itsVars->AppendElement(info);
		Broadcast(VarInserted(GetElementCount()));
		return kJTrue;
		}
}

/******************************************************************************
 AddFunction

 ******************************************************************************/

JBoolean
SCCircuitVarList::AddFunction
	(
	const JCharacter*	name,
	const JFunction&	f,
	const JBoolean		visible
	)
{
	JIndex index;
	if (!JNameValid(name))
		{
		return kJFalse;
		}
	else if (ParseVariableName(name, strlen(name), &index))
		{
		(JGetUserNotification())->ReportError("This variable name is already used.");
		return kJFalse;
		}
	else
		{
		VarInfo info;

		info.name = new JString(name);
		assert( info.name != NULL );

		info.f       = f.Copy();
		info.visible = visible;

		itsVars->AppendElement(info);
		Broadcast(VarInserted(GetElementCount()));
		return kJTrue;
		}
}

/******************************************************************************
 SetVariableName

 ******************************************************************************/

JBoolean
SCCircuitVarList::SetVariableName
	(
	const JIndex		varIndex,
	const JCharacter*	name
	)
{
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
		VarInfo info = itsVars->GetElement(varIndex);
		*(info.name) = name;
		Broadcast(VarNameChanged(varIndex));
		return kJTrue;
		}
}

/******************************************************************************
 SetValue

 ******************************************************************************/

JBoolean
SCCircuitVarList::SetValue
	(
	const JIndex	index,
	const JFloat	value
	)
{
	if (IsVariable(index))
		{
		VarInfo info = itsVars->GetElement(index);
		if (value != info.value)
			{
			info.value = value;
			itsVars->SetElement(index, info);
			Broadcast(VarValueChanged(index,1));
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetFunction

	Returns kJTrue if the specified variable is a function.

 ******************************************************************************/

JBoolean
SCCircuitVarList::SetFunction
	(
	const JIndex		index,
	const JFunction&	f
	)
{
	if (IsFunction(index))
		{
		VarInfo info = itsVars->GetElement(index);
		delete (info.f);
		info.f = f.Copy();
		itsVars->SetElement(index, info);
		Broadcast(VarValueChanged(index,1));
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Virtual functions for SCCircuitVarList class

	Not inline because they are virtual.

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
SCCircuitVarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	const VarInfo info = itsVars->GetElement(index);
	return *(info.name);
}

void
SCCircuitVarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	const JString* fullName    = (itsVars->GetElement(index)).name;
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
SCCircuitVarList::IsNumeric
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
SCCircuitVarList::IsDiscrete
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
SCCircuitVarList::IsArray
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
SCCircuitVarList::ArrayIndexValid
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
SCCircuitVarList::ValueIsKnown
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
SCCircuitVarList::GetUnknownValueSymbol
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
SCCircuitVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JFloat*			value
	)
	const
{
	const VarInfo info = itsVars->GetElement(variableIndex);
	if (elementIndex == 1 && info.f != NULL)
		{
		if (IsOnEvalStack(variableIndex))
			{
			*value = 0.0;
			return kJFalse;
			}
		PushOnEvalStack(variableIndex);
		const JBoolean ok = (info.f)->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
		}
	else if (elementIndex == 1)
		{
		*value = info.value;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
SCCircuitVarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	JComplex*		value
	)
	const
{
	const VarInfo info = itsVars->GetElement(variableIndex);
	if (elementIndex == 1 && info.f != NULL)
		{
		if (IsOnEvalStack(variableIndex))
			{
			*value = 0.0;
			return kJFalse;
			}
		PushOnEvalStack(variableIndex);
		const JBoolean ok = (info.f)->Evaluate(value);
		PopOffEvalStack(variableIndex);
		return ok;
		}
	else if (elementIndex == 1)
		{
		*value = info.value;
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
SCCircuitVarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JFloat	value
	)
{
	assert( elementIndex == 1 );
	const JBoolean ok = SetValue(variableIndex, value);
	assert( ok );
}

void
SCCircuitVarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	elementIndex,
	const JComplex&	value
	)
{
	assert( 0 /* SCCircuitVarList does not store complex values */ );
}

/******************************************************************************
 GetDiscreteValue

 ******************************************************************************/

JIndex
SCCircuitVarList::GetDiscreteValue
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
SCCircuitVarList::GetDiscreteValueCount
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
SCCircuitVarList::GetDiscreteValueName
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
SCCircuitVarList::SetDiscreteValue
	(
	const JIndex variableIndex,
	const JIndex elementIndex,
	const JIndex valueIndex
	)
{
	assert( 0 /* SCCircuitVarList has no discrete values */ );
}

/******************************************************************************
 Global functions for SCCircuitVarList class

 ******************************************************************************/

/******************************************************************************
 Stream operators

 ******************************************************************************/

ostream&
operator<<
	(
	ostream&				output,
	const SCCircuitVarList&	varList
	)
{
	const JSize varCount = (varList.itsVars)->GetElementCount();
	output << varCount;

	for (JIndex i=1; i<=varCount; i++)
		{
		const SCCircuitVarList::VarInfo info = (varList.itsVars)->GetElement(i);

		output << ' ' << *(info.name);
		output << ' ' << info.value;

		if (info.f != NULL)
			{
			const JString str = (info.f)->Print();
			output << ' ' << kJTrue << ' ' << str;
			}
		else
			{
			output << ' ' << kJFalse;
			}

		output << ' ' << info.visible;
		}

	// allow chaining

	return output;
}
