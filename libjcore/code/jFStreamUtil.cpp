/******************************************************************************
 jFStreamUtil.cpp

	Useful functions for dealing with fstreams.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <JString.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 JReadFile

	Read characters from the fstream until the end of the file is reached.
	These functions takes a JString* because the contents of the file
	could be very large, and returning a JString requires twice as much
	memory because of the copy constructor.

	These are equivalent to JReadAll() in jStreamUtil.h, except that they
	are optimized to work with file streams.

 ******************************************************************************/

void
JReadFile
	(
	const JCharacter*	fileName,
	JString*			str
	)
{
	ifstream input(fileName);
	JReadFile(input, str);
}

void
JReadFile
	(
	ifstream&	input,
	JString*	str
	)
{
	const JSize charCount = JGetFStreamLength(input) - JTellg(input);
	str->Read(input, charCount);
}

void
JReadFile
	(
	fstream&	input,
	JString*	str
	)
{
	const JSize charCount = JGetFStreamLength(input) - JTellg(input);
	str->Read(input, charCount);
}

/******************************************************************************
 JGetFStreamLength

	Returns the length of the file associated with theStream.

 ******************************************************************************/

JSize
JGetFStreamLength
	(
	ifstream& theStream
	)
{
	const long savedMark = theStream.tellg();
	theStream.seekg(0, ios::end);
	const JSize fileLength = theStream.tellg();
	theStream.seekg(savedMark, ios::beg);
	return fileLength;
}

JSize
JGetFStreamLength
	(
	fstream& theStream
	)
{
	const long savedMark = theStream.tellg();
	theStream.seekg(0, ios::end);
	const JSize fileLength = theStream.tellg();
	theStream.seekg(savedMark, ios::beg);
	return fileLength;
}

#if 0

// These functions cannot be implemented with the latest draft specification of fstream

/******************************************************************************
 JOpenScratchFile

	Open a temporary file and return it as an fstream.  Since we can't convince
	the filebuf to own the file, you must call JCloseScratchFile() to throw out
	the file properly.

 ******************************************************************************/

fstream*
JOpenScratchFile()
{
	FILE* tempFile = tmpfile();
	int fd = fileno(tempFile);
	assert( fd != EOF );

	fstream* theFile = new fstream(fd);
	assert( theFile != NULL );
	assert( theFile->good() );

	return theFile;
}

/******************************************************************************
 JCloseScratchFile

	Properly close a temporary file.

 ******************************************************************************/

void
JCloseScratchFile
	(
	fstream** theFile
	)
{
	(**theFile).close();
	delete *theFile;
	*theFile = NULL;
}

#endif
