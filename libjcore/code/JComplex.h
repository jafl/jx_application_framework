/******************************************************************************
 JComplex.h

	Defines clean complex number type

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JComplex
#define _H_JComplex

#include <complex>
#include "jTypes.h"

typedef std::complex<double>	JComplex;

class JString;

JComplex JASin(const JComplex&);
JComplex JACos(const JComplex&);
JComplex JATan(const JComplex&);

JComplex JASinh(const JComplex&);
JComplex JACosh(const JComplex&);
JComplex JATanh(const JComplex&);

enum JComplexDisplayMode
{
	kDisplayRealAndImag,
	kDisplayMagAndPhase
};

JComplexDisplayMode	JGetComplexDisplayMode();
void				JSetComplexDisplayMode(const JComplexDisplayMode mode);

JBoolean			JUsingEEImag();
void				JUseEEImag(const JBoolean useEEImag);
const JUtf8Byte*	JGetCurrentImagString();
const JUtf8Byte*	JGetMathImagString();
const JUtf8Byte*	JGetEEImagString();

JString	JPrintComplexNumber(const JComplex& value);
JString	JPrintComplexNumber(const JComplex& value,
							const JComplexDisplayMode mode,
							const JBoolean useEEImag);

#endif
