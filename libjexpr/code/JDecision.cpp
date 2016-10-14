/******************************************************************************
 JDecision.cpp

	This abstract class defines the requirements for all Decisions.

	BASE CLASS = none

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JDecision.h>
#include <jParseDecision.h>
#include <JString.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JDecision::JDecision
	(
	const JDecisionType type
	)
	:
	itsType( type )
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDecision::~JDecision()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDecision::JDecision
	(
	const JDecision& source
	)
	:
	itsType( source.itsType )
{
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given decision is identical to us.

 ******************************************************************************/

JBoolean
JDecision::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	return JConvertToBoolean( itsType == theDecision.itsType );
}

/******************************************************************************
 StreamIn (static)

	Parse the string to get the JDecision.

 ******************************************************************************/

JDecision*
JDecision::StreamIn
	(
	istream&		input,
	JVariableList*	theVariableList
	)
{
	JString expr;
	input >> expr;

	JDecision* theDecision;
	const JBoolean ok = JParseDecision(expr, theVariableList, &theDecision);
	assert( ok );
	return theDecision;
}

/******************************************************************************
 StreamOut

	Print the JDecision to the file as a JString.

 ******************************************************************************/

void
JDecision::StreamOut
	(
	ostream& output
	)
	const
{
	const JString str = Print();
	output << str;
}

/******************************************************************************
 Copy

	Create a copy of ourselves that uses a different JVariableList.

	The easiest way to do this is to print and then reparse ourselves
	using the new JVariableList.

 ******************************************************************************/

JDecision*
JDecision::Copy
	(
	JVariableList* newVariableList
	)
	const
{
	std::ostringstream expr;
	Print(expr);
	const std::string s = expr.str();

	JDecision* theDecision;
	const JBoolean ok = JParseDecision(s.c_str(), newVariableList, &theDecision);
	assert( ok );

	return theDecision;
}

/******************************************************************************
 Print

 ******************************************************************************/

JString
JDecision::Print()
	const
{
	std::ostringstream expr;
	Print(expr);
	return JString(expr.str());
}

/******************************************************************************
 JVariableList messages (virtual)

	The default is to do nothing.  Derived classes that store variable
	indicies must override all the functions.

 ******************************************************************************/

JBoolean
JDecision::UsesVariable
	(
	const JIndex variableIndex
	)
	const
{
	return kJFalse;
}

void
JDecision::VariablesInserted
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
}

void
JDecision::VariablesRemoved
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
}

void
JDecision::VariableMoved
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
}

void
JDecision::VariablesSwapped
	(
	const JIndex index1,
	const JIndex index2
	)
{
}

/******************************************************************************
 Cast to JBooleanDecision*

	Not inline because they are virtual

 ******************************************************************************/

JBooleanDecision*
JDecision::CastToJBooleanDecision()
{
	return NULL;
}

const JBooleanDecision*
JDecision::CastToJBooleanDecision()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JFunctionComparison*

	Not inline because they are virtual

 ******************************************************************************/

JFunctionComparison*
JDecision::CastToJFunctionComparison()
{
	return NULL;
}

const JFunctionComparison*
JDecision::CastToJFunctionComparison()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JDecisionWithVar*

	Not inline because they are virtual

 ******************************************************************************/

JDecisionWithVar*
JDecision::CastToJDecisionWithVar()
{
	return NULL;
}

const JDecisionWithVar*
JDecision::CastToJDecisionWithVar()
	const
{
	return NULL;
}

/******************************************************************************
 Global functions for JDecision class

 ******************************************************************************/

/******************************************************************************
 Comparison

 ******************************************************************************/

int
operator==
	(
	const JDecision& d1,
	const JDecision& d2
	)
{
	return d1.SameAs(d2);
}

int
operator!=
	(
	const JDecision& d1,
	const JDecision& d2
	)
{
	return !(d1 == d2);
}
