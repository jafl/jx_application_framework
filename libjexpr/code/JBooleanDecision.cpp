/******************************************************************************
 JBooleanDecision.cpp

								The JBooleanDecision Class

	This class provides routines for all 2 argument Boolean decisions.

	BASE CLASS = JBinaryDecision<JDecision>

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JBooleanDecision.h>
#include <JExprNodeList.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBooleanDecision::JBooleanDecision
	(
	const JCharacter*	name,
	const JDecisionType	type
	)
	:
	JBinaryDecision<JDecision>(name, type)
{
}

JBooleanDecision::JBooleanDecision
	(
	JDecision*			arg1,
	JDecision*			arg2,
	const JCharacter*	name,
	const JDecisionType	type
	)
	:
	JBinaryDecision<JDecision>(arg1, arg2, name, type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBooleanDecision::~JBooleanDecision()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBooleanDecision::JBooleanDecision
	(
	const JBooleanDecision& source
	)
	:
	JBinaryDecision<JDecision>(source)
{
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JBooleanDecision::Print
	(
	std::ostream& output
	)
	const
{
	PrintArg(output, GetArg1());
	output << GetName();
	PrintArg(output, GetArg2());
}

/******************************************************************************
 PrintArg

 ******************************************************************************/

void
JBooleanDecision::PrintArg
	(
	std::ostream&			output,
	const JDecision*	arg
	)
	const
{
	const JDecisionType argType = arg->GetType();
	if (argType != GetType() && argType != kJBooleanConstantType)
		{
		output << '(';
		arg->Print(output);
		output << ')';
		}
	else
		{
		arg->Print(output);
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JBooleanDecision::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	return JBinaryDecision<JDecision>::SameAsEitherOrder(theDecision);
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JBooleanDecision::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	nodeList->RecurseNodesForDecision(myNode, GetArg1());
	nodeList->RecurseNodesForDecision(myNode, GetArg2());
}

/******************************************************************************
 Cast to JBooleanDecision*

	Not inline because they are virtual

 ******************************************************************************/

JBooleanDecision*
JBooleanDecision::CastToJBooleanDecision()
{
	return this;
}

const JBooleanDecision*
JBooleanDecision::CastToJBooleanDecision()
	const
{
	return this;
}
