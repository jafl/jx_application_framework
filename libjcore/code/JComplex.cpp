/******************************************************************************
 JComplex.cpp

	Assembled complex number routines.

	Copyright © 1997-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JComplex.h>
#include <JString.h>
#include <jMath.h>
#include <jAssert.h>

using namespace std;

static const JCharacter* kMathImagString = "i";
static const JCharacter* kEEImagString   = "j";

// Private data

static JComplexDisplayMode theDisplayMode = kDisplayRealAndImag;
static JBoolean theUseEEImagFlag          = kJFalse;

/******************************************************************************
 JASin

	Complex inverse sine.

 ******************************************************************************/

JComplex
JASin
	(
	const JComplex& z
	)
{
	return JComplex(0,-1) * log(JComplex(0,1) * z + sqrt(1.0 - z*z));
}

/******************************************************************************
 JACos

	Complex inverse cosine.

 ******************************************************************************/

JComplex
JACos
	(
	const JComplex& z
	)
{
	return JComplex(0,-1) * log(z + JComplex(0,1) * sqrt(1.0 - z*z));
}

/******************************************************************************
 JATan

	Complex inverse tangent.

 ******************************************************************************/

JComplex
JATan
	(
	const JComplex& z
	)
{
	return JComplex(0,-0.5) * log((1.0 + JComplex(0,1) * z) / (1.0 - JComplex(0,1) * z));
}

/******************************************************************************
 JASinh

	Complex inverse hyperbolic sine.

 ******************************************************************************/

JComplex
JASinh
	(
	const JComplex& z
	)
{
	return log(z + sqrt(z*z + 1.0));
}

/******************************************************************************
 JACosh

	Principal value of the complex inverse hyperbolic cosine.

 ******************************************************************************/

JComplex
JACosh
	(
	const JComplex& z
	)
{
	return log(z + sqrt(z*z - 1.0));
}

/******************************************************************************
 JATanh

	Complex inverse hyperbolic tangent.

 ******************************************************************************/

JComplex
JATanh
	(
	const JComplex& z
	)
{
	return 0.5 * log((1.0 + z) / (1.0 - z));
}

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

JBoolean
JUsingEEImag()
{
	return theUseEEImagFlag;
}

void
JUseEEImag
	(
	const JBoolean useEEImag
	)
{
	theUseEEImagFlag = useEEImag;
}

/******************************************************************************
 Get imag strings

 ******************************************************************************/

const JCharacter*
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

const JCharacter*
JGetMathImagString()
{
	return kMathImagString;
}

const JCharacter*
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
	const JBoolean				useEEImag
	)
{
	JString str;
	if (mode == kDisplayRealAndImag)
		{
		const JFloat r = real(value);
		const JFloat i = imag(value);
		if (r == 0.0 && i == 0.0)
			{
			str += "0";
			}
		else if (r == HUGE_VAL || i == HUGE_VAL)
			{
			str += "Infinity";
			}
		else if (r == -HUGE_VAL || i == -HUGE_VAL)
			{
			str += "-Infinity";
			}
		else if (i == 0.0)
			{
			str += r;
			}
		else if (r == 0.0)
			{
			if (i == -1.0)
				{
				str += "- ";
				}
			else if (i != 1.0)
				{
				str += i;
				str += " ";
				}
			str += (useEEImag ? kEEImagString : kMathImagString);
			}
		else
			{
			str += r;
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
					str += fabs(i);
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
			str += "0";
			}
		else if (i == 0.0)
			{
			str += mag;
			}
		else
			{
			const JFloat ph     = arg(value) * kJRadToDeg;
			const JString phStr = ph;

			if (phStr == "180" || phStr == "-180")
				{
				str += "-";
				str += mag;
				}
			else
				{
				str += mag;
				str += ", ";
				str += ph;
				str += " deg";
				}
			}
		}

	return str;
}
