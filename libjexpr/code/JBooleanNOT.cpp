/******************************************************************************
 JBooleanNOT.cpp

	This class negates the result of its argument (JDecision).

	BASE CLASS = JDecision

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <JBooleanNOT.h>
#include <JExprNodeList.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JBooleanNOT::JBooleanNOT()
	:
	JDecision(kJBooleanNOTType)
{
	itsArg = NULL;
}

JBooleanNOT::JBooleanNOT
	(
	JDecision* arg
	)
	:
	JDecision(kJBooleanNOTType)
{
	itsArg = arg;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBooleanNOT::~JBooleanNOT()
{
	jdelete itsArg;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JBooleanNOT::JBooleanNOT
	(
	const JBooleanNOT& source
	)
	:
	JDecision(source)
{
	itsArg = (source.itsArg)->Copy();
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JDecision*
JBooleanNOT::Copy()
	const
{
	JBooleanNOT* newDecision = jnew JBooleanNOT(*this);
	assert( newDecision != NULL );
	return newDecision;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JBooleanNOT::Evaluate()
	const
{
	return !itsArg->Evaluate();
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JBooleanNOT::Print
	(
	std::ostream& output
	)
	const
{
	output << JPGetBooleanNOTString();

	const JDecisionType argType = itsArg->GetType();
	if (argType == kJBooleanANDType || argType == kJBooleanORType ||
		argType == kJBooleanXORType || argType == kJBooleanConstantType)
		{
		output << '(';
		itsArg->Print(output);
		output << ')';
		}
	else
		{
		itsArg->Print(output);
		}
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given function is identical to us.

 ******************************************************************************/

JBoolean
JBooleanNOT::SameAs
	(
	const JDecision& theDecision
	)
	const
{
	if (!JDecision::SameAs(theDecision))
		{
		return kJFalse;
		}

	const JBooleanNOT& theBooleanNOT = (const JBooleanNOT&) theDecision;
	return itsArg->SameAs(*(theBooleanNOT.itsArg));
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JBooleanNOT::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	nodeList->RecurseNodesForDecision(myNode, itsArg);
}
