/******************************************************************************
 jErrno.h

	Augments errno.h to hide the fact that multi-threaded applications
	must call a function to obtain the correct value of errno.

	Copyright © 1995-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jErrno
#define _H_jErrno

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <errno.h>
#include <jTypes.h>

inline int
jerrno()
{
	#if defined __GNUG__ || defined __KCC || \
		defined _MSC_VER || defined __SUNPRO_CC
		return errno;
	#else
		figure out what to do!
	#endif
}

inline void
jset_errno
	(
	const int err
	)
{
	#if defined __GNUG__ || defined __KCC || \
		defined _MSC_VER || defined __SUNPRO_CC
		errno = err;
	#else
		figure out what to do!
	#endif
}

inline void
jclear_errno()
{
	#if defined __GNUG__ || defined __KCC || \
		defined _MSC_VER || defined __SUNPRO_CC
		errno = 0;
	#else
		figure out what to do!
	#endif
}

inline JBoolean
jerrno_is_clear()
{
	return JConvertToBoolean( jerrno() == 0 );
}

#endif
