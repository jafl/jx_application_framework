/******************************************************************************
 jAssert.h

	Copyright (C) 1996-2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_jAssert
#define _H_jAssert

// regular assert macro

#undef assert
#undef assert_msg
#undef assert_ok

int JAssert(const char*, const char*, const int, const char* message);

#ifdef NDEBUG

	#define assert(x)			((void) 0)
	#define assert_msg(x, s)	((void) 0)
	#define assert_ok(x)		((void) 0)

#else

	#define assert(x)			((void) ((x) || JAssert(#x, __FILE__, __LINE__, "")))
	#define assert_msg(x, msg)	((void) ((x) || JAssert(#x, __FILE__, __LINE__, msg)))
	#define assert_ok(x)		((void) ((x).OK() || JAssert((x).GetMessage().GetBytes(), __FILE__, __LINE__, "")))

#endif

#endif

// memory manager - outside include fence

#include "jx-af/jcore/jNew.h"
