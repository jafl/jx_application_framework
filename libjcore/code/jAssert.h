/******************************************************************************
 jAssert.h

	Copyright (C) 1996-2017 by John Lindal.

 ******************************************************************************/

// memory manager - at the top, so our assert is used

#include "jNew.h"

// no fence - may need to override system macros multiple times

// regular assert macro

#undef assert
#undef assert_msg
#undef assert_ok

int JAssert(const char*, const char*, const int, const char* message, const char* function);

#ifdef NDEBUG

	#define assert(x)			((void) 0)
	#define assert_msg(x, s)	((void) 0)
	#define assert_ok(x)		((void) 0)

#else

	#include <boost/current_function.hpp>

	#define assert(x)			((void) ((x) || JAssert(#x, __FILE__, __LINE__, "", BOOST_CURRENT_FUNCTION)))
	#define assert_msg(x, msg)	((void) ((x) || JAssert(#x, __FILE__, __LINE__, msg, BOOST_CURRENT_FUNCTION)))
	#define assert_ok(x)		((void) ((x).OK() || JAssert((x).GetMessage().GetBytes(), __FILE__, __LINE__, "", BOOST_CURRENT_FUNCTION)))

#endif
