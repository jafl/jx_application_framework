/******************************************************************************
 jDeriv.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jDeriv
#define _H_jDeriv

#include <JFloat.h>

JFloat JCenterDiff2(const JFloat x, const JFloat dx, JFloat (fn)(const JFloat));
JFloat JCenterDiff4(const JFloat x, const JFloat dx, JFloat (fn)(const JFloat));
JFloat JCenterDiff6(const JFloat x, const JFloat dx, JFloat (fn)(const JFloat));

#endif
