/******************************************************************************
 JVector.cpp

	Class to represent a column vector.  To get a row vector, use transpose.

	This class was not designed to be a base class!  If you need to override it,
	be sure to make the destructor virtual.

	BASE CLASS = none

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JVector.h>
#include <JMatrix.h>
#include <jMath.h>
#include <jStreamUtil.h>
#include <string.h>
#include <stdarg.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The third form needs 3 arguments to distinguish it from the first form.

	*** The 3rd form does not promote types, so passing 0 will not work.
		You must enter 0.0 so it is passed as a JFloat.

 ******************************************************************************/

JVector::JVector
	(
	const JSize		dimCount,
	const JFloat	fillValue
	)
{
	assert( dimCount > 0 );

	JVectorX(dimCount);

	for (JIndex i=0; i<itsDimCount; i++)
		{
		itsElements[i] = fillValue;
		}
}

JVector::JVector
	(
	const JSize		dimCount,
	const JFloat	values[]
	)
{
	assert( dimCount > 0 );

	JVectorX(dimCount);

	for (JIndex i=0; i<itsDimCount; i++)
		{
		itsElements[i] = values[i];
		}
}

JVector::JVector
	(
	const JSize		dimCount,
	const JFloat	v1,
	const JFloat	v2,
	...
	)
{
	assert( dimCount >= 2 );

	JVectorX(dimCount);

	itsElements[0] = v1;
	itsElements[1] = v2;

	va_list argList;
	va_start(argList, v2);

	for (JIndex i=2; i<itsDimCount; i++)
		{
		itsElements[i] = va_arg(argList, JFloat);
		}

	va_end(argList);
}

// private

void
JVector::JVectorX
	(
	const JSize dimCount
	)
{
	itsDimCount = dimCount;

	itsElements = new JFloat [ itsDimCount ];
	assert( itsElements != NULL );
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JVector::JVector
	(
	const JVector& source
	)
{
	JVectorX(source.itsDimCount);

	memcpy(itsElements, source.itsElements, itsDimCount * sizeof(JFloat));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVector::~JVector()
{
	delete [] itsElements;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JVector&
JVector::operator=
	(
	const JVector& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	assert( JDimensionsEqual(*this, source) );

	memcpy(itsElements, source.itsElements, itsDimCount * sizeof(JFloat));

	// allow chaining

	return *this;
}

/******************************************************************************
 GetElement

 ******************************************************************************/

JFloat
JVector::GetElement
	(
	const JIndex index
	)
	const
{
	assert( 1 <= index && index <= itsDimCount );

	return itsElements [ index-1 ];
}

/******************************************************************************
 SetElement

 ******************************************************************************/

void
JVector::SetElement
	(
	const JIndex	index,
	const JFloat	value
	)
{
	assert( 1 <= index && index <= itsDimCount );

	itsElements [ index-1 ] = value;
}

/******************************************************************************
 IncrementElement

	Adds the given value to the specified element.

 ******************************************************************************/

void
JVector::IncrementElement
	(
	const JIndex	index,
	const JFloat	delta
	)
{
	assert( 1 <= index && index <= itsDimCount );

	itsElements [ index-1 ] += delta;
}

/******************************************************************************
 Negation

 ******************************************************************************/

JVector
JVector::operator-()
	const
{
	JVector resultVector(itsDimCount);

	for (JIndex i=0; i<itsDimCount; i++)
		{
		resultVector.itsElements[i] = - itsElements[i];
		}

	return resultVector;
}

/******************************************************************************
 Addition

 ******************************************************************************/

JVector&
JVector::operator+=
	(
	const JVector& v
	)
{
	assert( JDimensionsEqual(*this, v) );

	for (JIndex i=0; i<itsDimCount; i++)
		{
		itsElements[i] += v.itsElements[i];
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Subtraction

 ******************************************************************************/

JVector&
JVector::operator-=
	(
	const JVector& v
	)
{
	assert( JDimensionsEqual(*this, v) );

	for (JIndex i=0; i<itsDimCount; i++)
		{
		itsElements[i] -= v.itsElements[i];
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Multiplication by scalar

 ******************************************************************************/

JVector&
JVector::operator*=
	(
	const JFloat s
	)
{
	for (JIndex i=0; i<itsDimCount; i++)
		{
		itsElements[i] *= s;
		}

	// allow chaining

	return *this;
}

/******************************************************************************
 Division by scalar

 ******************************************************************************/

JVector&
JVector::operator/=
	(
	const JFloat s
	)
{
	assert( s != 0.0 );

	for (JIndex i=0; i<itsDimCount; i++)
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
JVector::SetAllElements
	(
	const JFloat value
	)
{
	for (JIndex i=0; i<itsDimCount; i++)
		{
		itsElements[i] = value;
		}
}

/******************************************************************************
 Set

	The number of elements passed in is assumed to be equal to
	the vector's size.

 ******************************************************************************/

void
JVector::Set
	(
	const JFloat values[]
	)
{
	memcpy(itsElements, values, itsDimCount * sizeof(JFloat));
}

void
JVector::Set
	(
	const JFloat v1,
	...
	)
{
	itsElements[0] = v1;

	va_list argList;
	va_start(argList, v1);

	for (JIndex i=1; i<itsDimCount; i++)
		{
		itsElements[i] = va_arg(argList, JFloat);
		}

	va_end(argList);
}

/******************************************************************************
 GetMinElement

	Returns the minimum element.  If index != NULL, it contains the index of
	the element that is returned.

 ******************************************************************************/

JFloat
JVector::GetMinElement
	(
	JIndex* index	// can be NULL
	)
	const
{
	JFloat min = 0.0;
	for (JIndex i=0; i<itsDimCount; i++)
		{
		if (itsElements[i] < min || i == 0)
			{
			min = itsElements[i];
			if (index != NULL)
				{
				*index = i+1;
				}
			}
		}

	return min;
}

/******************************************************************************
 GetMaxElement

	Returns the maximum element.  If index != NULL, it contains the index of
	the element that is returned.

 ******************************************************************************/

JFloat
JVector::GetMaxElement
	(
	JIndex* index	// can be NULL
	)
	const
{
	JFloat max = 0.0;
	for (JIndex i=0; i<itsDimCount; i++)
		{
		if (itsElements[i] > max || i == 0)
			{
			max = itsElements[i];
			if (index != NULL)
				{
				*index = i+1;
				}
			}
		}

	return max;
}

/******************************************************************************
 GetLength

	Return our Euclidean length.

 ******************************************************************************/

JFloat
JVector::GetLength()
	const
{
	JFloat normSq = 0.0;

	for (JIndex i=0; i<itsDimCount; i++)
		{
		normSq += itsElements[i] * itsElements[i];
		}

	return sqrt(normSq);
}

/******************************************************************************
 Transpose

 ******************************************************************************/

JMatrix
JVector::Transpose()
	const
{
	JMatrix vt(1, itsDimCount);

	for (JIndex i=0; i<itsDimCount; i++)
		{
		vt.SetElement(1,i+1, itsElements[i]);
		}

	return vt;
}

/******************************************************************************
 UnitVector

	Return a unit vector in the same direction as ourselves.

 ******************************************************************************/

JVector
JVector::UnitVector()
	const
{
	JVector unitVector = *this;
	unitVector.Normalize();
	return unitVector;
}

/******************************************************************************
 Global functions for JVector class

 ******************************************************************************/

/******************************************************************************
 Math

 ******************************************************************************/

JVector
operator+
	(
	const JVector& v1,
	const JVector& v2
	)
{
	JVector sum = v1;
	sum += v2;
	return sum;
}

JVector
operator-
	(
	const JVector& v1,
	const JVector& v2
	)
{
	JVector diff = v1;
	diff -= v2;
	return diff;
}

JVector
operator*
	(
	const JVector&	v,
	const JFloat	s
	)
{
	JVector result = v;
	result *= s;
	return result;
}

JVector
operator*
	(
	const JFloat	s,
	const JVector&	v
	)
{
	JVector result = v;
	result *= s;
	return result;
}

JVector
operator/
	(
	const JVector&	v,
	const JFloat	s
	)
{
	JVector result = v;
	result /= s;
	return result;
}

/******************************************************************************
 Dot product (inner product)

	Compute (v1)t.(v2)

 ******************************************************************************/

JFloat
JDotProduct
	(
	const JVector& v1,
	const JVector& v2
	)
{
	assert( JDimensionsEqual(v1, v2) );

	JFloat result = 0.0;
	const JSize dimCount = v1.GetDimensionCount();
	for (JIndex i=1; i<=dimCount; i++)
		{
		result += v1.GetElement(i) * v2.GetElement(i);
		}

	return result;
}

/******************************************************************************
 Cross product (vector product)

	Compute (v1)x(v2).

	*** This only works for vectors with 3 components.

 ******************************************************************************/

JVector
JCrossProduct
	(
	const JVector& v1,
	const JVector& v2
	)
{
	assert( v1.GetDimensionCount() == 3 &&
			v2.GetDimensionCount() == 3 );

	return JVector(3,
			v1.GetElement(2) * v2.GetElement(3) - v1.GetElement(3) * v2.GetElement(2),
			v1.GetElement(3) * v2.GetElement(1) - v1.GetElement(1) * v2.GetElement(3),
			v1.GetElement(1) * v2.GetElement(2) - v1.GetElement(2) * v2.GetElement(1));
}

/******************************************************************************
 Outer product

	Compute (v1).(v2)t

 ******************************************************************************/

JMatrix
JOuterProduct
	(
	const JVector& v1,
	const JVector& v2
	)
{
	const JSize rowCount = v1.GetDimensionCount();
	const JSize colCount = v2.GetDimensionCount();

	JMatrix resultMx(rowCount, colCount);

	for (JIndex i=1; i<=rowCount; i++)
		{
		const JFloat value1 = v1.GetElement(i);

		for (JIndex j=1; j<=colCount; j++)
			{
			resultMx.SetElement(i,j, value1 * v2.GetElement(j));
			}
		}

	return resultMx;
}

/******************************************************************************
 Comparison

 ******************************************************************************/

int
operator==
	(
	const JVector& v1,
	const JVector& v2
	)
{
	assert( JDimensionsEqual(v1, v2) );

	const JSize dimCount = v1.GetDimensionCount();
	for (JIndex i=1; i<=dimCount; i++)
		{
		if (v1.GetElement(i) != v2.GetElement(i))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JVector&	aVector
	)
{
	const JSize dimCount = aVector.GetDimensionCount();
	for (JIndex i=1; i<=dimCount; i++)
		{
		JFloat value;
		input >> value;
		aVector.SetElement(i, value);
		}

	// allow chaining

	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JVector&	aVector
	)
{
	const JSize dimCount = aVector.GetDimensionCount();
	for (JIndex i=1; i<=dimCount; i++)
		{
		const JFloat value = aVector.GetElement(i);

		if (i > 1)
			{
			output << ' ';
			}
		output << value;
		}

	// allow chaining

	return output;
}

#define JTemplateType JVector
#include <JPtrArray.tmpls>
#undef JTemplateType
