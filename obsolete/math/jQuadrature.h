/******************************************************************************
 jQuadrature.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jQuadrature
#define _H_jQuadrature

#include <JFloat.h>
#include <JSize.h>

JFloat JQuadratureSimpson(const JFloat xmin, const JFloat xmax, const JSize intCount,
						  JFloat (fn)(const JFloat));

#endif
