/******************************************************************************
 JComplex.h

	Defines clean complex number type

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JComplex
#define _H_JComplex

#include <complex>
#include "jTypes.h"

using JComplex = std::complex<double>;

class JString;

enum JComplexDisplayMode
{
	kDisplayRealAndImag,
	kDisplayMagAndPhase
};

JComplexDisplayMode	JGetComplexDisplayMode();
void				JSetComplexDisplayMode(const JComplexDisplayMode mode);

bool			JUsingEEImag();
void				JUseEEImag(const bool useEEImag);
const JUtf8Byte*	JGetCurrentImagString();
const JUtf8Byte*	JGetMathImagString();
const JUtf8Byte*	JGetEEImagString();

JString	JPrintComplexNumber(const JComplex& value);
JString	JPrintComplexNumber(const JComplex& value,
							const JComplexDisplayMode mode,
							const bool useEEImag);

#endif
