/******************************************************************************
 JInterPoly.cpp

	Class representing an interpolating polynomial.  We use the Newton form
	so it is computationally fast to add more points.

	The Newton table is stored as follows:

		f1
		f2 f12
		f3 f23 f123
		f4 f34 f234 f1234

	Thus, the offset to the start of the nth row is n*(n-1)/2.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JInterPoly.h>
#include <jassert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JInterPoly::JInterPoly()
{
	JInterPolyX();
}

JInterPoly::JInterPoly
	(
	const JArray<JFloat>& x,
	const JArray<JFloat>& y
	)
{
	JInterPolyX();
	AddPoints(x,y);
}

// private

void
JInterPoly::JInterPolyX()
{
	itsXList = new JArray<JFloat>(100);
	assert( itsXList != NULL );

	itsCoeffList = new JArray<JFloat>(100);
	assert( itsCoeffList != NULL );

	itsNewtonTable = new JArray<JFloat>(500);
	assert( itsNewtonTable != NULL );
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JInterPoly::JInterPoly
	(
	const JInterPoly& source
	)
{
	itsXList = new JArray<JFloat>(*(source.itsXList));
	assert( itsXList != NULL );

	itsCoeffList = new JArray<JFloat>(*(source.itsCoeffList));
	assert( itsCoeffList != NULL );

	itsNewtonTable = new JArray<JFloat>(*(source.itsNewtonTable));
	assert( itsNewtonTable != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JInterPoly::~JInterPoly()
{
	delete itsXList;
	delete itsCoeffList;
	delete itsNewtonTable;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JInterPoly&
JInterPoly::operator=
	(
	const JInterPoly& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	*itsXList       = *(source.itsXList);
	*itsCoeffList   = *(source.itsCoeffList);
	*itsNewtonTable = *(source.itsNewtonTable);

	return *this;
}

/******************************************************************************
 AddPoint

	Add the given point to the interpolation list.

 ******************************************************************************/

void
JInterPoly::AddPoint
	(
	const JFloat x,
	const JFloat y
	)
{
	if (itsXList->IsEmpty())
		{
		itsXList->AppendElement(x);
		itsCoeffList->AppendElement(y);
		itsNewtonTable->AppendElement(y);
		}
	else
		{
		const JSize ptCount = itsXList->GetElementCount();
		JIndex tableIndex = 1 + (ptCount * (ptCount-1) / 2);

		itsNewtonTable->AppendElement(y);
		JFloat lastv = y;
		for (JIndex i=ptCount; i>=1; i--)
			{
			const JFloat x1 = itsXList->GetElement(i);
			assert( x1 != x );
			const JFloat v =
				(lastv - itsNewtonTable->GetElement(tableIndex)) / (x - x1);
			itsNewtonTable->AppendElement(v);
			tableIndex++;
			lastv = v;
			}

		itsXList->AppendElement(x);
		itsCoeffList->AppendElement( itsNewtonTable->GetLastElement() );
		}
}

/******************************************************************************
 AddPoints

	Add the given points to the interpolation list.

 ******************************************************************************/

void
JInterPoly::AddPoints
	(
	const JArray<JFloat>& x,
	const JArray<JFloat>& y
	)
{
	const JSize ptCount = x.GetElementCount();
	assert( ptCount == y.GetElementCount() );

	for (JIndex i=1; i<=ptCount; i++)
		{
		AddPoint(x.GetElement(i), y.GetElement(i));
		}
}

/******************************************************************************
 Evaluate

	Evaluate the polynomial at the given point.

 ******************************************************************************/

// method 1
/*
JFloat
JInterPoly::Evaluate
	(
	const JFloat x
	)
	const
{
	if (itsXList->IsEmpty())
		{
		return 0.0;
		}
	else
		{
		const JSize ptCount = itsXList->GetElementCount();
		JFloat p = 1.0;
		JFloat f = itsCoeffList->GetFirstElement();
		for (JIndex i=2; i<=ptCount; i++)
			{
			p *= x - itsXList->GetElement(i-1);		// construct product incrementally
			f += itsCoeffList->GetElement(i) * p;	// this is the point of Newton's method
			}
		return f;
		}
}
*/

// method 2

JFloat
JInterPoly::Evaluate
	(
	const JFloat x
	)
	const
{
	if (itsXList->IsEmpty())
		{
		return 0.0;
		}
	else
		{
		const JSize ptCount = itsXList->GetElementCount();
		JFloat f = itsCoeffList->GetLastElement();
		for (JIndex i=ptCount-1; i>=1; i--)
			{
			f *= x - itsXList->GetElement(i);
			f += itsCoeffList->GetElement(i);
//			cout << i << ' ' << itsCoeffList->GetElement(i) << ' ' << f << endl;
			}
		return f;
		}
}
