/******************************************************************************
 JProbDistr.cpp

							The Probability Distribution Class

	This class stores a distribution of probabilities.

	BASE CLASS = none

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JProbDistr.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JProbDistr::JProbDistr
	(
	const JSize		elementCount,
	const JFloat	initialValue
	)
{
	// create array to hold the probabilities

	itsProbs = new JArray<JFloat>(elementCount);
	assert( itsProbs != NULL );

	// initialize the values

	for (JIndex i=1; i<=elementCount; i++)
		{
		itsProbs->AppendElement(initialValue);
		}
}

JProbDistr::JProbDistr
	(
	istream& input
	)
{
	// get the number of values

	JSize elementCount;
	input >> elementCount;

	// create array to hold the probabilities

	itsProbs = new JArray<JFloat>(elementCount);
	assert( itsProbs != NULL );

	// get the probabilities

	for (JIndex i=1; i<=elementCount; i++)
		{
		JFloat prob;
		input >> prob;
		itsProbs->AppendElement(prob);
		}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JProbDistr::JProbDistr
	(
	const JProbDistr& source
	)
{
	itsProbs = new JArray<JFloat>(*(source.itsProbs));
	assert( itsProbs != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JProbDistr::~JProbDistr()
{
	delete itsProbs;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JProbDistr&
JProbDistr::operator=
	(
	const JProbDistr& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	*(itsProbs) = *(source.itsProbs);

	return *this;
}

/******************************************************************************
 GetMostProbableElement

 ******************************************************************************/

JIndex
JProbDistr::GetMostProbableElement()
	const
{
	JIndex theIndex = 1;
	JFloat maxProb = GetProbability(1);

	const JSize elementCount = GetElementCount();
	for (JIndex i=2; i<=elementCount; i++)
		{
		const JFloat prob = GetProbability(i);
		if (prob > maxProb)
			{
			maxProb  = prob;
			theIndex = i;
			}
		}

	return theIndex;
}

/******************************************************************************
 operator +=

	If the given JProbDistr has the same number of values as we do, add the
	corresponding values.

 ******************************************************************************/

JProbDistr&
JProbDistr::operator+=
	(
	const JProbDistr& theProbDistr
	)
{
	// make sure that we have the same number of values

	const JSize elementCount = GetElementCount();
	assert( elementCount == theProbDistr.GetElementCount() );

	// add the new values to our values

	for (JIndex i=1; i<=elementCount; i++)
		{
		JFloat prob = GetProbability(i);
		prob += theProbDistr.GetProbability(i);
		SetProbability(i, prob);
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Normalize

	Scale our values so they are true probabilities (they sum to 1.0).
	Returns kJFalse if this is impossible.

 ******************************************************************************/

JBoolean
JProbDistr::Normalize()
{
JIndex i;

	const JSize elementCount = GetElementCount();

	const JFloat* prob = itsProbs->GetCArray();

	JFloat sum = 0.0;
	for (i=0; i<elementCount; i++)
		{
		sum += prob[i];
		}

	if (sum == 0.0)
		{
		return kJFalse;
		}

	for (i=1; i<=elementCount; i++)
		{
		SetProbability(i, prob[i-1] / sum);
		}

	return kJTrue;
}

/******************************************************************************
 Global functions for JProbDistr class

 ******************************************************************************/

/******************************************************************************
 Stream operators

 ******************************************************************************/

ostream&
operator<<
	(
	ostream&			output,
	const JProbDistr&	aProbDistr
	)
{
	// write the number of values

	const JSize elementCount = aProbDistr.GetElementCount();
	output << elementCount;

	// write the probabilities

	for (JIndex i=1; i<=elementCount; i++)
		{
		const JFloat prob = aProbDistr.GetProbability(i);
		output << ' ' << prob;
		}

	// allow chaining

	return output;
}
