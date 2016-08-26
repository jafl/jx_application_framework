/******************************************************************************
 jTime.h

	Augments time.h

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jTime
#define _H_jTime

#include <jTypes.h>
#include <time.h>

#if defined _J_UNIX && SIZEOF_LONG == 4
	#define J_TIME_T_MAX LONG_MAX
#elif defined _J_UNIX && SIZEOF_INT == 4
	#define J_TIME_T_MAX INT_MAX
#elif defined _MSC_VER && SIZEOF_LONG == 4
	#define J_TIME_T_MAX LONG_MAX
#else
	figure out what to do!
#endif

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000		// not declared in Sun headers
#endif

class JString;

void JWait(const double delta);

JString JGetTimeStamp();
JString JConvertToTimeStamp(const time_t t);
JString	JPrintTimeInterval(const JUInt delta);

void	JCheckExpirationDate(const time_t expireTime,
							 const JCharacter* map[], const JSize size);

long	JGetTimezoneOffset();

#endif
