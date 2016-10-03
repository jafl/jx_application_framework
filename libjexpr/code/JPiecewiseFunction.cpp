/******************************************************************************
 JPiecewiseFunction.cpp

							The JPiecewiseFunction Class

	This class represents a piecewise defined function.  We store a set of
	decisions and associated functions.  The first decision in the list that
	evaluates to true determines the function that is evaluated.  We store a
	separate default function in case none of the decisions are satisfied.

	BASE CLASS = none

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JPiecewiseFunction.h>
#include <JVariableList.h>
#include <JDecision.h>
#include <JFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JPiecewiseFunction::JPiecewiseFunction
	(
	JFunction* defaultFunction
	)
{
	itsCases = new JPtrArray<JDecision>(JPtrArrayT::kDeleteAll);
	assert( itsCases != NULL );

	itsFunctions = new JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
	assert( itsFunctions != NULL );

	itsDefaultFunction = defaultFunction;
}

JPiecewiseFunction::JPiecewiseFunction
	(
	istream&		input,
	JVariableList*	theVariableList
	)
{
	itsCases = new JPtrArray<JDecision>(JPtrArrayT::kDeleteAll);
	assert( itsCases != NULL );

	itsFunctions = new JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
	assert( itsFunctions != NULL );

	JSize caseCount;
	input >> caseCount;

	for (JIndex i=1; i<=caseCount; i++)
		{
		JDecision* newDecision = JDecision::StreamIn(input, theVariableList);
		itsCases->Append(newDecision);

		JFunction* newFunction = JFunction::StreamIn(input, theVariableList);
		itsFunctions->Append(newFunction);
		}

	itsDefaultFunction = JFunction::StreamIn(input, theVariableList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPiecewiseFunction::~JPiecewiseFunction()
{
	delete itsCases;
	delete itsFunctions;
	delete itsDefaultFunction;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JPiecewiseFunction::JPiecewiseFunction
	(
	const JPiecewiseFunction& source
	)
{
	itsCases = new JPtrArray<JDecision>(JPtrArrayT::kDeleteAll);
	assert( itsCases != NULL );

	itsFunctions = new JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
	assert( itsFunctions != NULL );

	const JSize caseCount = (source.itsCases)->GetElementCount();
	for (JIndex i=1; i<=caseCount; i++)
		{
		const JDecision* origDecision = (source.itsCases)->NthElement(i);
		JDecision* newDecision = origDecision->Copy();
		itsCases->Append(newDecision);

		const JFunction* origFunction = (source.itsFunctions)->NthElement(i);
		JFunction* newFunction = origFunction->Copy();
		itsFunctions->Append(newFunction);
		}

	itsDefaultFunction = (source.itsDefaultFunction)->Copy();
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JPiecewiseFunction::Evaluate
	(
	JFloat* value
	)
	const
{
	const JSize caseCount = itsCases->GetElementCount();
	for (JIndex i=1; i<=caseCount; i++)
		{
		JDecision* theDecision = itsCases->NthElement(i);
		if (theDecision->Evaluate())
			{
			JFunction* theFunction = itsFunctions->NthElement(i);
			return theFunction->Evaluate(value);
			}
		}

	return itsDefaultFunction->Evaluate(value);
}

/******************************************************************************
 SameAs

	Returns kJTrue if the given attribute is identical to us.
	Since we evaluate the first case that is satisfied, the cases must
	be in the same order for both objects.

 ******************************************************************************/

JBoolean
JPiecewiseFunction::SameAs
	(
	const JPiecewiseFunction& thePWFunction
	)
	const
{
	const JSize caseCount = itsCases->GetElementCount();
	if (caseCount != (thePWFunction.itsCases)->GetElementCount())
		{
		return kJFalse;
		}

	for (JIndex i=1; i<=caseCount; i++)
		{
		JDecision* d1 = itsCases->NthElement(i);
		JDecision* d2 = (thePWFunction.itsCases)->NthElement(i);
		if (*d1 != *d2)
			{
			return kJFalse;
			}
		JFunction* f1 = itsFunctions->NthElement(i);
		JFunction* f2 = (thePWFunction.itsFunctions)->NthElement(i);
		if (*f1 != *f2)
			{
			return kJFalse;
			}
		}

	return JConvertToBoolean( *itsDefaultFunction == *(thePWFunction.itsDefaultFunction) );
}

/******************************************************************************
 AddCase

	Appends a new case to the list of cases.  This new case becomes the case
	just before the default case.

 ******************************************************************************/

void
JPiecewiseFunction::AddCase
	(
	JDecision* theDecision,
	JFunction* theFunction
	)
{
	itsCases->Append(theDecision);
	itsFunctions->Append(theFunction);
}

/******************************************************************************
 SetDefaultFunction

	Change the default function.

 ******************************************************************************/

void
JPiecewiseFunction::SetDefaultFunction
	(
	JFunction* theFunction
	)
{
	delete itsDefaultFunction;
	itsDefaultFunction = theFunction;
}

/******************************************************************************
 Global functions for JPiecewiseFunction class

 ******************************************************************************/

/******************************************************************************
 Stream operators

 ******************************************************************************/

ostream&
operator<<
	(
	ostream&					output,
	const JPiecewiseFunction&	aPWFunc
	)
{
	// write the cases

	const JSize caseCount = (aPWFunc.itsCases)->GetElementCount();
	output << caseCount;

	for (JIndex i=1; i<=caseCount; i++)
		{
		JDecision* theDecision = (aPWFunc.itsCases)->NthElement(i);
		output << ' ';
		theDecision->StreamOut(output);

		JFunction* theFunction = (aPWFunc.itsFunctions)->NthElement(i);
		output << ' ';
		theFunction->StreamOut(output);
		}

	// write the default function

	output << ' ';
	(aPWFunc.itsDefaultFunction)->StreamOut(output);

	// allow chaining

	return output;
}
