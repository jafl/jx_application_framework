/******************************************************************************
 jAssert.h

	Interface for jAssert.cc

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jAssert
#define _H_jAssert

// regular assert macro

#undef assert

int JAssert(const char*, const char*, const int);

#ifdef NDEBUG

	#define assert(x)	((void) 0)

#else

	#define assert(x)	((void) ((x) || JAssert(#x, __FILE__, __LINE__)))

#endif

// JError assert macro

#undef assert_ok

#ifdef NDEBUG

	#define assert_ok(x)	((void) 0)

#else

	#define assert_ok(x)	((void) ((x).OK() || JAssert((x).GetMessage(), __FILE__, __LINE__)))

#endif

#endif

// memory manager

#include <jNew.h>
