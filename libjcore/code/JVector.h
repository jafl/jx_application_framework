/******************************************************************************
 JVector.h

	Interface for the JVector class

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVector
#define _H_JVector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JMatrix;

class JVector
{
public:

	JVector(const JSize dimCount, const JFloat fillValue = 0.0);
	JVector(const JSize dimCount, const JFloat values[]);
	JVector(const JSize dimCount, const JFloat v1, const JFloat v2, ...);
	JVector(const JVector& source);

	~JVector();

	const JVector& operator=(const JVector&);

	JVector& operator+=(const JVector&);
	JVector& operator-=(const JVector&);
	JVector& operator*=(const JFloat);
	JVector& operator/=(const JFloat);

	JVector operator-() const;

	JSize	GetDimensionCount() const;
	JFloat	GetElement(const JIndex index) const;
	void	SetElement(const JIndex index, const JFloat value);
	void	IncrementElement(const JIndex index, const JFloat delta);
	void	SetAllElements(const JFloat value);

	const JFloat*	GetElements() const;
	void			Set(const JFloat values[]);
	void			Set(const JFloat v1, ...);

	JFloat	GetMinElement(JIndex* index = NULL) const;
	JFloat	GetMaxElement(JIndex* index = NULL) const;

	JFloat	GetLength() const;

	JMatrix	Transpose() const;
	void	Normalize();
	JVector	UnitVector() const;

private:

	JSize	itsDimCount;
	JFloat*	itsElements;

private:

	void	JVectorX(const JSize dimCount);
};

// declarations of global functions for dealing with vectors

JVector operator+(const JVector&, const JVector&);
JVector operator-(const JVector&, const JVector&);

JVector operator*(const JVector&, const JFloat);
JVector operator*(const JFloat, const JVector&);
JVector operator/(const JVector&, const JFloat);

JFloat  JDotProduct(const JVector&, const JVector&);
JVector JCrossProduct(const JVector&, const JVector&);
JMatrix JOuterProduct(const JVector&, const JVector&);

istream& operator>>(istream&, JVector&);
ostream& operator<<(ostream&, const JVector&);

int operator==(const JVector&, const JVector&);

inline int
operator!=
	(
	const JVector& v1,
	const JVector& v2
	)
{
	return !(v1 == v2);
}

/******************************************************************************
 GetDimensionCount

 ******************************************************************************/

inline JSize
JVector::GetDimensionCount()
	const
{
	return itsDimCount;
}

/******************************************************************************
 JDimensionsEqual

 ******************************************************************************/

inline JBoolean
JDimensionsEqual
	(
	const JVector& v1,
	const JVector& v2
	)
{
	return JConvertToBoolean( v1.GetDimensionCount() == v2.GetDimensionCount() );
}

/******************************************************************************
 GetElements

 ******************************************************************************/

inline const JFloat*
JVector::GetElements()
	const
{
	return itsElements;
}

/******************************************************************************
 Normalize

 ******************************************************************************/

inline void
JVector::Normalize()
{
	*this /= GetLength();
}

#endif
