/******************************************************************************
 JBooleanConstant.cpp

							The Boolean Constant Class

	This class represents a constant boolean value (True or False).

	BASE CLASS = JDecision

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JBooleanConstant.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBooleanConstant::JBooleanConstant
	(
	const JBoolean value
	)
	:
	JDecision(kJBooleanConstantType)
{
	itsValue = value;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBooleanConstant::~JBooleanConstant()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBooleanConstant::JBooleanConstant
	(
	const JBooleanConstant& source
	)
	:
	JDecision(source)
{
	itsValue = source.itsValue;
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JBooleanConstant::Copy()
	const
{
	JBooleanConstant* newDecision = new JBooleanConstant(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JBooleanConstant::Evaluate()
	const
{
	return itsValue;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JBooleanConstant::Print
	(
	ostream& output
	)
	const
{
	if (itsValue)
		{
		output << JPGetTrueConstDecisionString();
		}
	else
		{
		output << JPGetFalseConstDecisionString();
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JBooleanConstant::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (!JDecision::SameAs(theDecision))
		{
		return kJFalse;
		}

	const JBooleanConstant& theBoolConst = (const JBooleanConstant&) theDecision;
	return JConvertToBoolean( itsValue == theBoolConst.itsValue );
}

/******************************************************************************
 BuildNodeList

	We are a terminal node.

 ******************************************************************************/

void
JBooleanConstant::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
}
