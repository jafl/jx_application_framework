/******************************************************************************
 JInterPoly.h

	Interface for the JInterPoly class.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JInterPoly
#define _H_JInterPoly

#include <JArray.h>
#include <JFloat.h>

class JInterPoly
{
public:

	JInterPoly();
	JInterPoly(const JArray<JFloat>& x, const JArray<JFloat>& y);
	JInterPoly(const JInterPoly& source);

	~JInterPoly();

	const JInterPoly& operator=(const JInterPoly& source);

	JSize	GetPointCount() const;
	void	AddPoint(const JFloat x, const JFloat y);
	void	AddPoints(const JArray<JFloat>& x, const JArray<JFloat>& y);

	JFloat	operator() (const JFloat x) const;
	JFloat	Evaluate(const JFloat x) const;

private:

	JArray<JFloat>*	itsXList;
	JArray<JFloat>*	itsCoeffList;
	JArray<JFloat>*	itsNewtonTable;

private:

	void	JInterPolyX();
};


/******************************************************************************
 GetPointCount

	Returns the number of interpolation points.

 ******************************************************************************/

inline JSize
JInterPoly::GetPointCount()
	const
{
	return itsXList->GetElementCount();
}

/******************************************************************************
 operator()

	Evaluates the interpolating polynomial at x.

 ******************************************************************************/

inline JFloat
JInterPoly::operator()
	(
	const JFloat x
	)
	const
{
	return Evaluate(x);
}

#endif
