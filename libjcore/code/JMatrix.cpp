/******************************************************************************
 JMatrix.cpp

	This class was not designed to be a base class!  If you need to override it,
	be sure to make the destructor virtual.

	BASE CLASS = none

	Copyright © 1994-95 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JMatrix.h>
#include <JVector.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <string.h>
#include <stdarg.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JMatrix::JMatrix
	(
	const JSize		rowCount,
	const JSize		colCount,
	const JFloat	fillValue
	)
{
	JMatrixX(rowCount, colCount);
	SetAllElements(fillValue);
}

JMatrix::JMatrix
	(
	const JVector& v
	)
{
	JMatrixX(v.GetDimensionCount(), 1);
	SetColVector(1, v);
}

// private

void
JMatrix::JMatrixX
	(
	const JSize rowCount,
	const JSize colCount
	)
{
	assert( rowCount > 0 );
	assert( colCount > 0 );

	itsRowCount = rowCount;
	itsColCount = colCount;

	itsElements = new JFloat [ itsRowCount * itsColCount ];
	assert( itsElements != NULL );
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JMatrix::JMatrix
	(
	const JMatrix& source
	)
{
	JMatrixX(source.itsRowCount, source.itsColCount);

	memcpy(itsElements, source.itsElements, itsRowCount * itsColCount * sizeof(JFloat));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JMatrix::~JMatrix()
{
	delete [] itsElements;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JMatrix&
JMatrix::operator=
	(
	const JMatrix& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	assert( JDimensionsEqual(*this, source) );

	memcpy(itsElements, source.itsElements, itsRowCount * itsColCount * sizeof(JFloat));

	// allow chaining

	return *this;
}

/******************************************************************************
 GetElement

 ******************************************************************************/

JFloat
JMatrix::GetElement
	(
	const JIndex rowIndex,
	const JIndex colIndex
	)
	const
{
	assert( RowIndexValid(rowIndex) && ColIndexValid(colIndex) );

	return itsElements[ RCToOffset(rowIndex, colIndex) ];
}

/******************************************************************************
 SetElement

 ******************************************************************************/

void
JMatrix::SetElement
	(
	const JIndex	rowIndex,
	const JIndex	colIndex,
	const JFloat	value
	)
{
	assert( RowIndexValid(rowIndex) && ColIndexValid(colIndex) );

	itsElements[ RCToOffset(rowIndex, colIndex) ] = value;
}

/******************************************************************************
 IncrementElement

	Adds the given value to the specified element.

 ******************************************************************************/

void
JMatrix::IncrementElement
	(
	const JIndex	rowIndex,
	const JIndex	colIndex,
	const JFloat	delta
	)
{
	assert( RowIndexValid(rowIndex) && ColIndexValid(colIndex) );

	itsElements[ RCToOffset(rowIndex, colIndex) ] += delta;
}

/******************************************************************************
 Negation

 ******************************************************************************/

JMatrix
JMatrix::operator-()
	const
{
	JMatrix mx(itsRowCount, itsColCount);

	const JSize count = GetElementCount();
	for (JIndex i=0; i<count; i++)
		{
		mx.itsElements[i] = -itsElements[i];
		}

	// allow chaining

	return mx;
}

/******************************************************************************
 Addition

 ******************************************************************************/

JMatrix&
JMatrix::operator+=
	(
	const JMatrix& mx
	)
{
	assert( JDimensionsEqual(*this, mx) );

	const JSize count = GetElementCount();
	for (JIndex i=0; i<count; i++)
		{
		itsElements[i] += mx.itsElements[i];
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Subtraction

 ******************************************************************************/

JMatrix&
JMatrix::operator-=
	(
	const JMatrix& mx
	)
{
	assert( JDimensionsEqual(*this, mx) );

	const JSize count = GetElementCount();
	for (JIndex i=0; i<count; i++)
		{
		itsElements[i] -= mx.itsElements[i];
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Multiplication by scalar

 ******************************************************************************/

JMatrix&
JMatrix::operator*=
	(
	const JFloat s
	)
{
	const JSize count = GetElementCount();
	for (JIndex i=0; i<count; i++)
		{
		itsElements[i] *= s;
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Division by scalar

 ******************************************************************************/

JMatrix&
JMatrix::operator/=
	(
	const JFloat s
	)
{
	assert( s != 0.0 );

	const JSize count = GetElementCount();
	for (JIndex i=0; i<count; i++)
		{
		itsElements[i] /= s;
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 SetAllElements

 ******************************************************************************/

void
JMatrix::SetAllElements
	(
	const JFloat value
	)
{
	const JSize count = GetElementCount();
	for (JIndex i=0; i<count; i++)
		{
		itsElements[i] = value;
		}
}

/******************************************************************************
 GetRowVector

	Return the specified row as a JVector.

 ******************************************************************************/

JVector
JMatrix::GetRowVector
	(
	const JIndex rowIndex
	)
	const
{
	JVector rowVector(itsColCount);

	for (JIndex j=1; j<=itsColCount; j++)
		{
		rowVector.SetElement(j, GetElement(rowIndex,j));
		}

	return rowVector;
}

/******************************************************************************
 SetRowVector

	Replace the specified row with the given JVector.

 ******************************************************************************/

void
JMatrix::SetRowVector
	(
	const JIndex	rowIndex,
	const JVector&	rowVector
	)
{
	assert( itsColCount == rowVector.GetDimensionCount() );

	for (JIndex j=1; j<=itsColCount; j++)
		{
		SetElement(rowIndex,j, rowVector.GetElement(j));
		}
}

/******************************************************************************
 GetColVector

	Return the specified column as a JVector.

 ******************************************************************************/

JVector
JMatrix::GetColVector
	(
	const JIndex colIndex
	)
	const
{
	return JVector(itsRowCount, itsElements + RCToOffset(1,colIndex));
}

/******************************************************************************
 SetColVector

	Replace the specified column with the given JVector.

 ******************************************************************************/

void
JMatrix::SetColVector
	(
	const JIndex	colIndex,
	const JVector&	colVector
	)
{
	assert( itsRowCount == colVector.GetDimensionCount() );

	for (JIndex i=1; i<=itsRowCount; i++)
		{
		SetElement(i,colIndex, colVector.GetElement(i));
		}
}

/******************************************************************************
 SetRow

	Replace the specified row with the given values.

 ******************************************************************************/

void
JMatrix::SetRow
	(
	const JIndex	rowIndex,
	const JFloat	v1,
	...
	)
{
	SetElement(rowIndex, 1, v1);

	va_list argList;
	va_start(argList, v1);

	for (JIndex i=2; i<=itsColCount; i++)
		{
		SetElement(rowIndex, i, va_arg(argList, JFloat));
		}

	va_end(argList);
}

/******************************************************************************
 SetCol

	Replace the specified column with the given values.

 ******************************************************************************/

void
JMatrix::SetCol
	(
	const JIndex	colIndex,
	const JFloat	v1,
	...
	)
{
	SetElement(1, colIndex, v1);

	va_list argList;
	va_start(argList, v1);

	for (JIndex i=2; i<=itsRowCount; i++)
		{
		SetElement(i, colIndex, va_arg(argList, JFloat));
		}

	va_end(argList);
}

/******************************************************************************
 SetElements

	Fills the matrix with the given values in row major order.

 ******************************************************************************/

void
JMatrix::SetElements
	(
	const JFloat v1,
	...
	)
{
	SetElement(1, 1, v1);

	va_list argList;
	va_start(argList, v1);

	for (JIndex i=1; i<=itsRowCount; i++)
		{
		for (JIndex j=1; j<=itsColCount; j++)
			{
			if (i > 1 || j > 1)
				{
				SetElement(i, j, va_arg(argList, JFloat));
				}
			}
		}

	va_end(argList);
}

/******************************************************************************
 Transpose

 ******************************************************************************/

JMatrix
JMatrix::Transpose()
	const
{
	JMatrix mx(itsColCount, itsRowCount);

	for (JIndex i=1; i<=itsRowCount;i++)
		{
		for (JIndex j=1; j<=itsColCount;j++)
			{
			mx.SetElement(j,i, GetElement(i,j));
			}
		}

	return mx;
}

/******************************************************************************
 Invert

	Returns kJTrue if we were able to calculate an inverse.

 ******************************************************************************/

JBoolean
JMatrix::Invert
	(
	JMatrix* inverse
	)
	const
{
	// we can only be inverted if we are a square matrix

	if (itsRowCount != itsColCount)
		{
		return kJFalse;
		}

	// deal with the trivial case

	if (itsRowCount == 1)
		{
		if (itsElements[0] != 0.0)
			{
			inverse->itsElements[0] = 1.0/itsElements[0];
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}

	// Perform row reduction on mx.  *inverse then contains the desired result.

	JMatrix mx = *this;
	*inverse   = JIdentityMatrix(itsRowCount);
	return JGaussianElimination(&mx, inverse);
}

/******************************************************************************
 Determinant

 ******************************************************************************/

JFloat
JMatrix::Determinant()
	const
{
	// we can only calculate the determinant if we are a square matrix

	if (itsRowCount != itsColCount)
		{
		return kJFalse;
		}

	// deal with the trivial case

	if (itsRowCount == 1)
		{
		return itsElements[0];
		}

	// create workspace to hold row reduction results

	JMatrix mx = *this;

	// Perform row reduction on mx to get upper triangular matrix

	long sign = 1;
	{
	for (JIndex i=1; i<itsRowCount; i++)
		{
		const JIndex pivotRow   = mx.FindPivotRow(i);
		const JFloat pivotValue = mx.GetElement(pivotRow,i);

		// if the largest element is zero, the determinant is zero

		if (pivotValue == 0.0)
			{
			return 0.0;
			}

		// swap rows and flip sign of determinant

		if (pivotRow != i)
			{
			mx.SwapRows(i, pivotRow);
			sign = -sign;
			}

		// wipe out the part of ith column that is below the diagonal

		for (JIndex k=i+1; k<=itsRowCount; k++)
			{
			const JFloat scaleFactor = -mx.GetElement(k,i) / pivotValue;
			if (scaleFactor != 0.0)
				{
				mx.AddRowToRow(i, scaleFactor, k);
				}
			}
		}
	}

	// determinant of upper triangular mx is product of elements on diagonal

	JFloat det = sign;
	{
	for (JIndex i=1; i<=itsRowCount; i++)
		{
		det *= mx.GetElement(i,i);
		}
	}
	return det;
}

/******************************************************************************
 SwapRows (protected)

 ******************************************************************************/

void
JMatrix::SwapRows
	(
	const JIndex index1,
	const JIndex index2
	)
{
	for (JIndex j=1; j<=itsColCount; j++)
		{
		const JFloat temp = GetElement(index1,j);
		SetElement(index1,j, GetElement(index2,j));
		SetElement(index2,j, temp);
		}
}

/******************************************************************************
 ScaleRow (protected)

 ******************************************************************************/

void
JMatrix::ScaleRow
	(
	const JIndex	index,
	const JFloat	scaleFactor
	)
{
	for (JIndex j=1; j<=itsColCount; j++)
		{
		SetElement(index,j, GetElement(index,j) * scaleFactor);
		}
}

/******************************************************************************
 AddRowToRow (protected)

 ******************************************************************************/

void
JMatrix::AddRowToRow
	(
	const JIndex	sourceIndex,
	const JFloat	scaleFactor,
	const JIndex	destIndex
	)
{
	for (JIndex j=1; j<=itsColCount; j++)
		{
		IncrementElement(destIndex,j, GetElement(sourceIndex,j) * scaleFactor);
		}
}

/******************************************************************************
 FindPivotRow (protected)

	Given the diagonal element that is under consideration, search all the
	elements in this column below the diagonal to find the largest one.

 ******************************************************************************/

JIndex
JMatrix::FindPivotRow
	(
	const JIndex rowIndex
	)
	const
{
	JFloat pivotValue = GetElement(rowIndex, rowIndex);
	JIndex pivotRow   = rowIndex;

	for (JIndex i=rowIndex+1; i<=itsRowCount; i++)
		{
		const JFloat value = GetElement(i,rowIndex);
		if (fabs(value) > fabs(pivotValue))
			{
			pivotValue = value;
			pivotRow   = i;
			}
		}

	return pivotRow;
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JMatrix::Print
	(
	ostream& output
	)
	const
{
	for (JIndex i=1; i<=itsRowCount; i++)
		{
		for (JIndex j=1; j<=itsColCount; j++)
			{
			output << GetElement(i,j);
			if (j < itsColCount)
				{
				output << "  ";
				}
			}
		output << endl;
		}
}

/******************************************************************************
 Global functions for JMatrix class

 ******************************************************************************/

/******************************************************************************
 Math

 ******************************************************************************/

JMatrix
operator+
	(
	const JMatrix& mx1,
	const JMatrix& mx2
	)
{
	JMatrix sum = mx1;
	sum += mx2;
	return sum;
}

JMatrix
operator-
	(
	const JMatrix& mx1,
	const JMatrix& mx2
	)
{
	JMatrix diff = mx1;
	diff -= mx2;
	return diff;
}

JMatrix
operator*
	(
	const JMatrix&	mx,
	const JFloat	s
	)
{
	JMatrix result = mx;
	result *= s;
	return result;
}

JMatrix
operator*
	(
	const JFloat	s,
	const JMatrix&	mx
	)
{
	JMatrix result = mx;
	result *= s;
	return result;
}

JMatrix
operator/
	(
	const JMatrix&	mx,
	const JFloat	s
	)
{
	JMatrix result = mx;
	result /= s;
	return result;
}

/******************************************************************************
 Matrix products

 ******************************************************************************/

JMatrix
operator*
	(
	const JMatrix& mx1,
	const JMatrix& mx2
	)
{
	// make sure multiplication is defined

	assert( mx1.GetColCount() == mx2.GetRowCount() );

	// compute the product

	const JSize rowCount = mx1.GetRowCount();
	const JSize colCount = mx2.GetColCount();

	JMatrix resultMx(rowCount, colCount);

	const JSize termCount = mx1.GetColCount();
	for (JIndex i=1; i<=rowCount; i++)
		{
		for (JIndex j=1;j<=colCount;j++)
			{
			JFloat value = 0.0;
			for (JIndex k=1; k<=termCount; k++)
				{
				value += mx1.GetElement(i,k) * mx2.GetElement(k,j);
				}

			resultMx.SetElement(i,j, value);
			}
		}

	return resultMx;
}

JMatrix
operator*
	(
	const JMatrix& mx,
	const JVector& v
	)
{
	// make sure multiplication is defined

	assert( mx.GetColCount() == v.GetDimensionCount() );

	// compute the product

	const JSize rowCount = mx.GetRowCount();

	JMatrix resultMx(rowCount, 1);

	const JSize termCount = mx.GetColCount();
	for (JIndex i=1; i<=rowCount; i++)
		{
		JFloat value = 0.0;
		for (JIndex k=1; k<=termCount; k++)
			{
			value += mx.GetElement(i,k) * v.GetElement(k);
			}

		resultMx.SetElement(i,1, value);
		}

	return resultMx;
}

/******************************************************************************
 JIdentityMatrix

 ******************************************************************************/

JMatrix
JIdentityMatrix
	(
	const JSize dimCount
	)
{
	JMatrix identityMx(dimCount, dimCount, 0.0);
	for (JIndex i=1;i<=dimCount;i++)
		{
		identityMx.SetElement(i,i, 1.0);
		}

	return identityMx;
}

/******************************************************************************
 Comparison

 ******************************************************************************/

int
operator==
	(
	const JMatrix& mx1,
	const JMatrix& mx2
	)
{
	// make sure we are dealing with matrices of the same size

	assert( JDimensionsEqual(mx1, mx2) );

	// compare the two matrices

	const JSize rowCount = mx1.GetRowCount();
	const JSize colCount = mx1.GetColCount();

	for (JIndex i=1; i<=rowCount; i++)
		{
		for (JIndex j=1; j<=colCount; j++)
			{
			if (mx1.GetElement(i,j) != mx2.GetElement(i,j))
				{
				return 0;
				}
			}
		}

	return 1;
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JMatrix&	aMatrix
	)
{
	const JSize rowCount = aMatrix.GetRowCount();
	const JSize colCount = aMatrix.GetColCount();

	for (JIndex i=1; i<=rowCount; i++)
		{
		for (JIndex j=1; j<=colCount; j++)
			{
			JFloat value;
			input >> value;
			aMatrix.SetElement(i,j, value);
			}

		if (i < rowCount)
			{
			char c;
			input >> ws >> c;
			if (c != ',')
				{
				JSetState(input, ios::failbit);
				return input;
				}
			}
		}

	// allow chaining

	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JMatrix&	aMatrix
	)
{
	const JSize rowCount = aMatrix.GetRowCount();
	const JSize colCount = aMatrix.GetColCount();

	for (JIndex i=1; i<=rowCount; i++)
		{
		for (JIndex j=1; j<=colCount; j++)
			{
			const JFloat value = aMatrix.GetElement(i,j);

			if (i != 1 || j != 1)
				{
				output << ' ';
				}
			output << value;
			}

		if (i < rowCount)
			{
			output << ',';
			}
		}

	// allow chaining

	return output;
}

/******************************************************************************
 JGaussianElimination

	Returns kJTrue if we were able to perform Gaussian elimination on A|b
	to reduce A to the identity.  The result of b is stored in x.

	A does not have to be square.  A, b, x must all have the same number of
	rows.  b and x must have the same number of columns.

 ******************************************************************************/

JBoolean
JGaussianElimination
	(
	const JMatrix&	A,
	const JVector&	b,
	JVector*		x
	)
{
	JMatrix A1 = A;
	JMatrix x1 = b;
	if (JGaussianElimination(&A1, &x1))
		{
		*x = x1.GetColVector(1);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JGaussianElimination
	(
	const JMatrix&	A,
	const JMatrix&	b,
	JMatrix*		x
	)
{
	JMatrix A1 = A;
	*x         = b;
	return JGaussianElimination(&A1, x);
}

/******************************************************************************
 JGaussianElimination

	Returns kJTrue if we were able to perform Gaussian elimination on A|x.
	A and x are both modified.  If the process fails, the partial results
	are returned.

	A does not have to be square.  A and x must all have the same number of rows.

 ******************************************************************************/

JBoolean
JGaussianElimination
	(
	JMatrix* A,
	JMatrix* x
	)
{
	const JSize rowCount = A->GetRowCount();
	assert( rowCount == x->GetRowCount() );

	// deal with the trivial case

	if (rowCount == 1)
		{
		JFloat scaleFactor = A->GetElement(1,1);
		if (scaleFactor != 0.0)
			{
			scaleFactor = 1.0/scaleFactor;
			A->ScaleRow(1, scaleFactor);
			x->ScaleRow(1, scaleFactor);
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}

	// Perform row reduction on A.  x then contains the desired result.

	for (JIndex i=1; i<=rowCount; i++)
		{
		const JIndex pivotRow   = A->FindPivotRow(i);
		const JFloat pivotValue = A->GetElement(pivotRow,i);

		// If the largest element is zero, the matrix is not reducible.

		if (pivotValue == 0.0)
			{
			return kJFalse;
			}

		// swap rows

		if (pivotRow != i)
			{
			A->SwapRows(i, pivotRow);
			x->SwapRows(i, pivotRow);
			}

		// normalize mx(i,i) to 1

		JFloat scaleFactor = 1.0/pivotValue;
		A->ScaleRow(i, scaleFactor);
		x->ScaleRow(i, scaleFactor);

		// wipe out the ith column in mx

		for (JIndex k=1; k<=rowCount; k++)
			{
			if (k != i)
				{
				scaleFactor = - A->GetElement(k,i);
				if (scaleFactor != 0.0)
					{
					A->AddRowToRow(i, scaleFactor, k);
					A->SetElement(k,i, 0.0);				// just to be sure
					x->AddRowToRow(i, scaleFactor, k);
					}
				}
			}
		}

	return kJTrue;
}

#define JTemplateType JMatrix
#include <JPtrArray.tmpls>
#undef JTemplateType
