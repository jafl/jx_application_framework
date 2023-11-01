#ifndef _H_jNew
#define _H_jNew

/******************************************************************************
 jNew.h

	The JCore replacement for operator new.  These functions are simply an
	interface to the appropriate JMemoryManager functions.

	2016/01/05: The latest C++ standard, allows "= delete" to be appended
	to a function declaration.  This broke the original "delete" macro.

	Copyright (C) 1997 by Dustin Laurence.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <stdlib.h> // For size_t
#include "jTypes.h"

void* operator new(size_t size, const JUtf8Byte* file, const JUInt32 line, const int type) noexcept;
void* operator new[](size_t size, const JUtf8Byte* file, const JUInt32 line, const int type) noexcept;

void JLocateDelete(const JUtf8Byte* file, const JUInt32 line);

#ifndef _J_MEMORY_BUCKET
#define _J_MEMORY_BUCKET 0
#endif

#define jnew new(__FILE__, __LINE__, _J_MEMORY_BUCKET)
#define jdelete JLocateDelete(__FILE__, __LINE__), delete

#endif
