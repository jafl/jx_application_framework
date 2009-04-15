/******************************************************************************
 jFStreamUtil_UNIX.cpp

	UNIX System routines to do what fstreams ought to be able to do
	in the first place.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jFStreamUtil.h>
#include <fcntl.h>
#include <unistd.h>
#include <jMissingProto.h>
#include <jAssert.h>

/******************************************************************************
 JSetFStreamLength

	Sets the length of the file associated with originalStream.

	The only efficient way is to use ftruncate(), which requires a file
	descriptor.  Since we can't get this from the stream, we require
	the file name.  Once we operate on the descriptor, the stream will
	be hopelessly confused, so we have to close it first.  This is why
	we return a new one afterwards.

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

	const int fd = open(fileName, O_RDWR);
	assert( fd != -1 );

	int err = ftruncate(fd, newLength);
	assert( err == 0 );

	err = close(fd);
	assert( err == 0 );

	// open a new stream for the file and return a pointer to it

	fstream* newStream = new fstream(fileName, io_mode);
	assert( newStream != NULL );

	return newStream;
}
