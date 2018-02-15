/******************************************************************************
 JDiagBandMatrix.h

	Interface for the JDiagBandMatrix class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JDiagBandMatrix
#define _H_JDiagBandMatrix

#include <JPtrArray.h>
#include <JFloat.h>
#include <JSignedIndex.h>

class JVector;

class JDiagBandMatrix
{
	friend JBoolean Solve(const JDiagBandMatrix& A, const JVector& b,
						  JVector* x);

public:

	JDiagBandMatrix(const JSize size, const JSize lowerBandCount,
					const JSize upperBandCount);
	JDiagBandMatrix(const JDiagBandMatrix& source);

	~JDiagBandMatrix();

	const JDiagBandMatrix& operator=(const JDiagBandMatrix& source);

	JSize		GetSize() const;
	JBoolean	IndexValid(const JIndex index) const;

	JFloat	GetElement(const JIndex rowIndex, const JIndex columnIndex) const;
	void	SetElement(const JIndex rowIndex, const JIndex columnIndex,
					   const JFloat value);
	void	SetBand(const JSignedIndex bandIndex, const JFloat value);

private:

	typedef JArray<JFloat>	Band;

private:

	JSize				itsSize;			// matrix dimensions
	JSize				itsLowerBandCount;
	JSize				itsUpperBandCount;
	JPtrArray<Band>*	itsBands;

private:

	void	ScaleRow(const JIndex rowIndex, const JFloat scaleFactor);
	void	AddRowToRow(const JIndex sourceIndex, const JFloat scaleFactor,
						const JIndex destIndex);
};


/******************************************************************************
 GetSize

 ******************************************************************************/

inline JSize
JDiagBandMatrix::GetSize()
	const
{
	return itsSize;
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
JDiagBandMatrix::IndexValid
	(
	const JIndex index
	)
	const
{
	return ConvertToBoolean( 1 <= index && index <= itsSize );
}

#endif
