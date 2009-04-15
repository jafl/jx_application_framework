/******************************************************************************
 JMatrix.h

	Interface for the JMatrix class

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JMatrix
#define _H_JMatrix

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JVector;

class JMatrix
{
	friend JBoolean	JGaussianElimination(const JMatrix& A, const JVector& b, JVector* x);
	friend JBoolean	JGaussianElimination(const JMatrix& A, const JMatrix& b, JMatrix* x);
	friend JBoolean	JGaussianElimination(JMatrix* A, JMatrix* x);

public:

	JMatrix(const JSize rowCount, const JSize colCount,
			const JFloat fillValue = 0.0);
	JMatrix(const JVector&);
	JMatrix(const JMatrix&);

	~JMatrix();

	const JMatrix& operator=(const JMatrix&);

	JMatrix& operator+=(const JMatrix&);
	JMatrix& operator-=(const JMatrix&);
	JMatrix& operator*=(const JFloat);
	JMatrix& operator/=(const JFloat);

	JMatrix operator-() const;

	JSize	GetRowCount() const;
	JSize	GetColCount() const;
	JSize	GetElementCount() const;

	JFloat	GetElement(const JIndex rowIndex, const JIndex colIndex) const;
	void	SetElement(const JIndex rowIndex, const JIndex colIndex,
					   const JFloat value);
	void	IncrementElement(const JIndex rowIndex, const JIndex colIndex,
							 const JFloat delta);
	void	SetAllElements(const JFloat value);

	void	SetRow(const JIndex rowIndex, const JFloat v1, ...);
	void	SetCol(const JIndex colIndex, const JFloat v1, ...);
	void	SetElements(const JFloat v1, ...);

	JVector	GetRowVector(const JIndex rowIndex) const;
	void	SetRowVector(const JIndex rowIndex, const JVector& rowVector);
	JVector	GetColVector(const JIndex colIndex) const;
	void	SetColVector(const JIndex colIndex, const JVector& colVector);

	JMatrix		Transpose() const;
	JBoolean	Invert(JMatrix* inverse) const;
	JFloat		Determinant() const;

	void	Print(ostream& output) const;

	JBoolean	RowIndexValid(const JIndex rowIndex) const;
	JBoolean	ColIndexValid(const JIndex colIndex) const;

protected:

	void	SwapRows(const JIndex index1, const JIndex index2);
	void	ScaleRow(const JIndex rowIndex, const JFloat scaleFactor);
	void	AddRowToRow(const JIndex sourceIndex, const JFloat scaleFactor,
						const JIndex destIndex);

	JIndex	FindPivotRow(const JIndex rowIndex) const;

private:

	JSize	itsRowCount;
	JSize	itsColCount;
	JFloat* itsElements;

private:

	void	JMatrixX(const JSize rowCount, const JSize colCount);
	JSize	RCToOffset(const JIndex rowIndex, const JIndex colIndex) const;
};

// declarations of global functions for dealing with matrices

JMatrix operator+(const JMatrix&, const JMatrix&);
JMatrix operator-(const JMatrix&, const JMatrix&);

JMatrix operator*(const JMatrix&, const JMatrix&);
JMatrix operator*(const JMatrix&, const JVector&);
JMatrix operator*(const JMatrix&, const JFloat);
JMatrix operator*(const JFloat, const JMatrix&);
JMatrix operator/(const JMatrix&, const JFloat);

JMatrix JIdentityMatrix(const JSize dimCount);

istream& operator>>(istream&, JMatrix&);
ostream& operator<<(ostream&, const JMatrix&);

int operator==(const JMatrix&, const JMatrix&);

inline int
operator!=
	(
	const JMatrix& mx1,
	const JMatrix& mx2
	)
{
	return !(mx1 == mx2);
}

/******************************************************************************
 Get dimensions

 ******************************************************************************/

inline JSize
JMatrix::GetRowCount()
	const
{
	return itsRowCount;
}

inline JSize
JMatrix::GetColCount()
	const
{
	return itsColCount;
}

inline JSize
JMatrix::GetElementCount()
	const
{
	return itsRowCount * itsColCount;
}

/******************************************************************************
 JDimensionsEqual

 ******************************************************************************/

inline JBoolean
JDimensionsEqual
	(
	const JMatrix& mx1,
	const JMatrix& mx2
	)
{
	return JConvertToBoolean( mx1.GetRowCount() == mx2.GetRowCount() &&
							  mx1.GetColCount() == mx2.GetColCount() );
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
JMatrix::RowIndexValid
	(
	const JIndex rowIndex
	)
	const
{
	return JConvertToBoolean( 1 <= rowIndex && rowIndex <= itsRowCount );
}

inline JBoolean
JMatrix::ColIndexValid
	(
	const JIndex colIndex
	)
	const
{
	return JConvertToBoolean( 1 <= colIndex && colIndex <= itsColCount );
}

/******************************************************************************
 RCToOffset (private)

 ******************************************************************************/

inline JSize
JMatrix::RCToOffset
	(
	const JIndex rowIndex,
	const JIndex colIndex
	)
	const
{
	return (colIndex-1) * itsRowCount + (rowIndex-1);
}

#endif
