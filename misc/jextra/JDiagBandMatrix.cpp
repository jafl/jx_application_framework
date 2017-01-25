/******************************************************************************
 JDiagBandMatrix.cpp

	Class representing a square, banded matrix with bands around the diagonal.
	Bands are indexed by zero for the diagonal, increasingly negative values
	below the diagonal, and increasingly positive values above the diagonal.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JDiagBandMatrix.h>
#include <JVector.h>
#include <JMinMax.h>
#include <jassert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JDiagBandMatrix::JDiagBandMatrix
	(
	const JSize size,
	const JSize lowerBandCount,
	const JSize upperBandCount
	)
{
	assert( lowerBandCount < size && upperBandCount < size );

	itsSize           = size;
	itsLowerBandCount = lowerBandCount;
	itsUpperBandCount = upperBandCount;

	const JSize bandCount = 1 + itsLowerBandCount + itsUpperBandCount;
	itsBands = new JPtrArray<Band>(JPtrArrayT::kDeleteAll, bandCount);
	assert( itsBands != NULL );

	for (JIndex i=1; i<=bandCount; i++)
		{
		Band* band = new Band(itsSize);
		assert( band != NULL );
		itsBands->Append(band);

		for (JIndex j=1; j<=itsSize; j++)
			{
			band->AppendElement(0.0);
			}
		}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDiagBandMatrix::JDiagBandMatrix
	(
	const JDiagBandMatrix& source
	)
{
	itsSize           = source.itsSize;
	itsLowerBandCount = source.itsLowerBandCount;
	itsUpperBandCount = source.itsUpperBandCount;

	const JSize bandCount = (source.itsBands)->GetElementCount();
	itsBands = new JPtrArray<Band>(bandCount);
	assert( itsBands != NULL );

	for (JIndex i=1; i<=bandCount; i++)
		{
		Band* origBand = (source.itsBands)->GetElement(i);
		Band* band = new Band(*origBand);
		assert( band != NULL );
		itsBands->Append(band);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDiagBandMatrix::~JDiagBandMatrix()
{
	delete itsBands;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JDiagBandMatrix&
JDiagBandMatrix::operator=
	(
	const JDiagBandMatrix& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	assert( itsSize == source.itsSize );

	itsLowerBandCount = source.itsLowerBandCount;
	itsUpperBandCount = source.itsUpperBandCount;

	itsBands->DeleteAll();
	const JSize bandCount = (source.itsBands)->GetElementCount();

	for (JIndex i=1; i<=bandCount; i++)
		{
		Band* origBand = (source.itsBands)->GetElement(i);
		Band* band = new Band(*origBand);
		assert( band != NULL );
		itsBands->Append(band);
		}

	return *this;
}

/******************************************************************************
 GetElement

 ******************************************************************************/

JFloat
JDiagBandMatrix::GetElement
	(
	const JIndex rowIndex,
	const JIndex columnIndex
	)
	const
{
	assert( IndexValid(rowIndex) && IndexValid(columnIndex) );

	if (columnIndex - itsUpperBandCount <= rowIndex &&
		rowIndex <= columnIndex + itsLowerBandCount)
		{
		Band* band = itsBands->GetElement(itsLowerBandCount+1+columnIndex-rowIndex);
		return band->GetElement(columnIndex);
		}
	else
		{
		return 0.0;
		}
}

/******************************************************************************
 SetElement

 ******************************************************************************/

void
JDiagBandMatrix::SetElement
	(
	const JIndex	rowIndex,
	const JIndex	columnIndex,
	const JFloat	value
	)
{
	assert( IndexValid(rowIndex) && IndexValid(columnIndex) );

	if (columnIndex - itsUpperBandCount <= rowIndex &&
		rowIndex <= columnIndex + itsLowerBandCount)
		{
		Band* band = itsBands->GetElement(itsLowerBandCount+1+columnIndex-rowIndex);
		band->SetElement(columnIndex, value);
		}
	else
		{
		assert( 0 );	// operation not allowed yet
		}
}

/******************************************************************************
 SetBand

 ******************************************************************************/

void
JDiagBandMatrix::SetBand
	(
	const JSignedIndex	bandIndex,
	const JFloat		value
	)
{
	assert( -(JSignedIndex)itsLowerBandCount <= bandIndex &&
			bandIndex <= (JSignedIndex)itsUpperBandCount );

	Band* band = itsBands->GetElement(itsLowerBandCount+1+bandIndex);
	for (JIndex i=1; i<=itsSize; i++)
		{
		band->SetElement(i, value);
		}
}

/******************************************************************************
 ScaleRow (private)

 ******************************************************************************/

void
JDiagBandMatrix::ScaleRow
	(
	const JIndex	rowIndex,
	const JFloat	scaleFactor
	)
{
	JIndex minCol = 1;
	if (rowIndex > itsLowerBandCount)
		{
		minCol = rowIndex - itsLowerBandCount;
		}

	const JIndex maxCol = JMin(rowIndex + itsUpperBandCount, itsSize);

	for (JIndex i=minCol; i<=maxCol; i++)
		{
		SetElement(rowIndex,i, scaleFactor * GetElement(rowIndex,i));
		}
}

/******************************************************************************
 AddRowToRow (private)

	*** Since we have only allocated memory for the bands, destination
		elements outside the bands will remain zero.

 ******************************************************************************/

void
JDiagBandMatrix::AddRowToRow
	(
	const JIndex	sourceIndex,
	const JFloat	scaleFactor,
	const JIndex	destIndex
	)
{
	JIndex minSrcCol = 1;
	if (sourceIndex > itsLowerBandCount)
		{
		minSrcCol = sourceIndex - itsLowerBandCount;
		}

	const JIndex maxSrcCol = JMin(sourceIndex + itsUpperBandCount, itsSize);

	JIndex minDestCol = 1;
	if (destIndex > itsLowerBandCount)
		{
		minDestCol = destIndex - itsLowerBandCount;
		}

	const JIndex maxDestCol = JMin(destIndex + itsUpperBandCount, itsSize);

	const JIndex minCol = JMax(minSrcCol, minDestCol);
	const JIndex maxCol = JMin(maxSrcCol, maxDestCol);

	for (JIndex i=minCol; i<=maxCol; i++)
		{
		SetElement(destIndex,i, GetElement(destIndex,i) +
				   scaleFactor * GetElement(sourceIndex,i));
		}
}

/******************************************************************************
 Global functions for JDiagBandMatrix class

 ******************************************************************************/

/******************************************************************************
 Solve

	Returns kJTrue if we were able to find a unique solution.
	We can't use pivoting because that could move elements outside the
	allocated bands.

 ******************************************************************************/

JBoolean
Solve
	(
	const JDiagBandMatrix&	A,
	const JVector&			b,
	JVector*				x
	)
{
JIndex i;

	*x = b;

	// deal with the trivial case

	const JSize rowCount = A.GetSize();
	if (rowCount == 1)
		{
		const JFloat value = A.GetElement(1,1);
		if (value != 0.0)
			{
			*x /= value;
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}

	// create workspace to hold row reduction results

	JDiagBandMatrix mx = A;

	// Pass 1:  eliminate bands below the diagonal

	for (i=1; i<=rowCount; i++)
		{
		// normalize mx(i,i) to 1

		JFloat scaleFactor = mx.GetElement(i,i);
		if (scaleFactor == 0.0)
			{
			return kJFalse;
			}

		mx.ScaleRow(i, 1.0/scaleFactor);
		x->SetElement(i, x->GetElement(i) / scaleFactor);

		// wipe out lower values in the ith column in mx

		const JSize jmax = JMin(i+mx.itsLowerBandCount, rowCount);
		for (JIndex j=i+1; j<=jmax; j++)
			{
			scaleFactor = -mx.GetElement(j,i);
			if (scaleFactor != 0.0)
				{
				mx.AddRowToRow(i, scaleFactor, j);
				mx.SetElement(j,i, 0.0);				// just to be sure
				x->SetElement(j, x->GetElement(j) + scaleFactor * x->GetElement(i));
				}
			}
		}

	// Pass 2:  eliminate bands above the diagonal

	for (i=rowCount; i>=2; i--)
		{
		JSize jmin = 1;
		if (i > mx.itsUpperBandCount)
			{
			jmin = i - mx.itsUpperBandCount;
			}

		for (JIndex j=i-1; j>=jmin; j--)
			{
			JFloat scaleFactor = -mx.GetElement(j,i);
			if (scaleFactor != 0.0)
				{
				mx.AddRowToRow(i, scaleFactor, j);
				mx.SetElement(j,i, 0.0);				// just to be sure
				x->SetElement(j, x->GetElement(j) + scaleFactor * x->GetElement(i));
				}
			}
		}

	return kJTrue;
}
