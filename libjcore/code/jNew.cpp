/******************************************************************************
 jNew.cpp

	JNew.cc defines the JCore operator new, JNew.  JNew provides many memory
	management features that the built-in allocator does not, principally for
	detection of memory leaks.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JMemoryManager.h>

// Module header
#include <jNew.h>

/******************************************************************************
 operator new

 *****************************************************************************/

void*
operator new
	(
	size_t size
	)
	noexcept
{
	return operator new(size, "<UNKNOWN>", 0);
}

/******************************************************************************
 operator new

 *****************************************************************************/

void*
operator new
	(
	size_t             size,
	const  JCharacter* file,
	const  JUInt32     line
	)
	noexcept
{
	return JMemoryManager::New(size, file, line, kJFalse);
}

/******************************************************************************
 operator new[]

 *****************************************************************************/

void*
operator new[]
	(
	size_t size
	)
	noexcept
{
	return operator new[](size, "<UNKNOWN>", 0);
}

/******************************************************************************
 operator new[]

 *****************************************************************************/

void*
operator new[]
	(
	size_t             size,
	const  JCharacter* file,
	const  JUInt32     line
	)
	noexcept
{
	return JMemoryManager::New(size, file, line, kJTrue);
}

/******************************************************************************
 operator delete

 *****************************************************************************/

void
operator delete
	(
	void* memory
	)
	noexcept
{
	JMemoryManager::Instance()->Delete(memory, kJFalse);
}

/******************************************************************************
 operator delete[]

 *****************************************************************************/

void
operator delete[]
	(
	void* memory
	)
	noexcept
{
	JMemoryManager::Instance()->Delete(memory, kJTrue);
}

/******************************************************************************
 JLocateDelete

 *****************************************************************************/

void
JLocateDelete
	(
	const JCharacter* file,
	const JUInt32     line
	)
{
	JMemoryManager::Instance()->LocateDelete(file, line);
}
