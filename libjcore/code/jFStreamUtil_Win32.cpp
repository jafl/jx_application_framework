/******************************************************************************
 jFStreamUtil_Win32.cpp

	Windows System routines to do what fstreams ought to be able to do
	in the first place.

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jFStreamUtil.h>
#include <windows.h>
#include <jAssert.h>

/******************************************************************************
 JSetFStreamLength

	Sets the length of the file associated with originalStream.

	The only efficient way is to use SetEndOfFile(), which requires a file
	handle.  Since we can't get this from the stream, we require the file
	name.  Once we operate on the handle, the stream will be hopelessly
	confused, so we have to close it first.  This is why we return a new
	one afterwards.

	The caller is responsible for deleting originalStream.

 ******************************************************************************/

fstream*
JSetFStreamLength
	(
	const char*				fileName,
	fstream&				originalStream,
	const JSize				newLength,
	const JFStreamOpenMode	io_mode
	)
{
	originalStream.close();

	HANDLE h = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
						  OPEN_EXISTING, 0, NULL);
	assert( h != INVALID_HANDLE_VALUE );

	const DWORD result = SetFilePointer(h, newLength, NULL, FILE_BEGIN);
	assert( result != INVALID_SET_FILE_POINTER );

	const BOOL success = SetEndOfFile(h);
	assert( success );

	CloseHandle(h);

	// open a new stream for the file and return a pointer to it

	fstream* newStream = new fstream(fileName, io_mode);
	assert( newStream != NULL );

	return newStream;
}
