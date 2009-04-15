/******************************************************************************
 JComplex.h

	Defines clean complex number type

	Copyright © 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JComplex
#define _H_JComplex

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <complex>
#include <jTypes.h>

#ifdef __KCC
typedef complex<double>	JComplex;
#elif __SUNPRO_CC == 0x420
typedef complex	JComplex;
#else
typedef std::complex<double>	JComplex;
#endif

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
const JCharacter*	JGetCurrentImagString();
const JCharacter*	JGetMathImagString();
const JCharacter*	JGetEEImagString();

JString	JPrintComplexNumber(const JComplex& value);
JString	JPrintComplexNumber(const JComplex& value,
							const JComplexDisplayMode mode,
							const JBoolean useEEImag);

#endif
