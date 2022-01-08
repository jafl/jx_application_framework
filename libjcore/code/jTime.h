/******************************************************************************
 jTime.h

	Augments time.h

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_jTime
#define _H_jTime

#include "jx-af/jcore/jTypes.h"
#include <time.h>

class JString;

void JWait(const double delta);

JString JGetTimeStamp();
JString JConvertToTimeStamp(const time_t t);
JString	JPrintTimeInterval(const JUInt delta);

void	JCheckExpirationDate(const time_t expireTime,
							 const JUtf8Byte* map[], const JSize size);

long	JGetTimezoneOffset();

#endif
