/******************************************************************************
 JComplex.cpp

	Assembled complex number routines.

	Copyright (C) 1997-99 John Lindal.

 ******************************************************************************/

#include "JComplex.h"
#include "JString.h"
#include "jMath.h"
#include "jAssert.h"

using namespace std;

static const JUtf8Byte* kMathImagString = "i";
static const JUtf8Byte* kEEImagString   = "j";

// Private data

static JComplexDisplayMode theDisplayMode = kDisplayRealAndImag;
static bool theUseEEImagFlag          = false;

/******************************************************************************
 Flag to set display mode

 ******************************************************************************/

JComplexDisplayMode
JGetComplexDisplayMode()
{
	return theDisplayMode;
}

void
JSetComplexDisplayMode
	(
	const JComplexDisplayMode mode
	)
{
	theDisplayMode = mode;
}

/******************************************************************************
 Flag to set use of i vs j

 ******************************************************************************/

bool
JUsingEEImag()
{
	return theUseEEImagFlag;
}

void
JUseEEImag
	(
	const bool useEEImag
	)
{
	theUseEEImagFlag = useEEImag;
}

/******************************************************************************
 Get imag strings

 ******************************************************************************/

const JUtf8Byte*
JGetCurrentImagString()
{
	if (theUseEEImagFlag)
		{
		return kEEImagString;
		}
	else
		{
		return kMathImagString;
		}
}

const JUtf8Byte*
JGetMathImagString()
{
	return kMathImagString;
}

const JUtf8Byte*
JGetEEImagString()
{
	return kEEImagString;
}

/******************************************************************************
 JPrintComplexNumber

 ******************************************************************************/

JString
JPrintComplexNumber
	(
	const JComplex& value
	)
{
	return JPrintComplexNumber(value, theDisplayMode, theUseEEImagFlag);
}

JString
JPrintComplexNumber
	(
	const JComplex&				value,
	const JComplexDisplayMode	mode,
	const bool				useEEImag
	)
{
	JString str;
	if (mode == kDisplayRealAndImag)
		{
		const JFloat r = real(value);
		const JFloat i = imag(value);
		if (r == 0.0 && i == 0.0)
			{
			str = "0";
			}
		else if (r == HUGE_VAL || i == HUGE_VAL)
			{
			str = "Infinity";
			}
		else if (r == -HUGE_VAL || i == -HUGE_VAL)
			{
			str = "-Infinity";
			}
		else if (i == 0.0)
			{
			str = JString(r);
			}
		else if (r == 0.0)
			{
			if (i == -1.0)
				{
				str = "-";
				}
			else if (i != 1.0)
				{
				str  = JString(i);
				str += " ";
				}
			str += (useEEImag ? kEEImagString : kMathImagString);
			}
		else
			{
			str = JString(r);
			if (i == -1.0)
				{
				str += " - ";
				}
			else
				{
				if (i > 0.0)
					{
					str += " + ";
					}
				else
					{
					str += " - ";
					}

				if (i != 1.0)
					{
					str += JString(fabs(i));
					str += " ";
					}
				}
			str += (useEEImag ? kEEImagString : kMathImagString);
			}
		}

	else if (mode == kDisplayMagAndPhase)
		{
		const JFloat mag = abs(value);
		const JFloat i   = imag(value);
		if (mag == 0.0)
			{
			str = "0";
			}
		else if (i == 0.0 && real(value) > 0.0)
			{
			str = JString(mag);
			}
		else
			{
			const JFloat ph = arg(value) * kJRadToDeg;
			const JString phStr(ph);

			if (phStr == "180" || phStr == "-180")
				{
				str  = "-";
				str += JString(mag);
				}
			else
				{
				str  = JString(mag);
				str += ", ";
				str += phStr;
				str += " deg";
				}
			}
		}

	return str;
}
