/******************************************************************************
 jQuadrature.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jQuadrature
#define _H_jQuadrature

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFloat.h>
#include <JSize.h>

JFloat JQuadratureSimpson(const JFloat xmin, const JFloat xmax, const JSize intCount,
						  JFloat (fn)(const JFloat));

#endif
