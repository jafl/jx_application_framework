/******************************************************************************
 jFStreamUtil.cpp

	Useful functions for dealing with fstreams.

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <JString.h>
#include <stdio.h>
#include <jAssert.h>

/******************************************************************************
 JReadFile

	Read characters from the std::fstream until the end of the file is reached.
	These functions takes a JString* because the contents of the file
	could be very large, and returning a JString requires twice as much
	memory because of the copy constructor.

	These are equivalent to JReadAll() in jStreamUtil.h, except that they
	are optimized to work with file streams.

 ******************************************************************************/

void
JReadFile
	(
	const JString&	fileName,
	JString*		str
	)
{
	std::ifstream input(fileName.GetBytes());
	JReadFile(input, str);
}

void
JReadFile
	(
	std::ifstream&	input,
	JString*	str
	)
{
	const JSize charCount = JGetFStreamLength(input) - JTellg(input);
	str->Read(input, charCount);
}

void
JReadFile
	(
	std::fstream&	input,
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
	std::ifstream& theStream
	)
{
	const long savedMark = theStream.tellg();
	theStream.seekg(0, std::ios::end);
	const JSize fileLength = theStream.tellg();
	theStream.seekg(savedMark, std::ios::beg);
	return fileLength;
}

JSize
JGetFStreamLength
	(
	std::fstream& theStream
	)
{
	const long savedMark = theStream.tellg();
	theStream.seekg(0, std::ios::end);
	const JSize fileLength = theStream.tellg();
	theStream.seekg(savedMark, std::ios::beg);
	return fileLength;
}

/******************************************************************************
 JConvertToStream

	Convert the data from the given file descriptor into an std::ifstream.
	The location of the file is returned in tempFullName.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JConvertToStream
	(
	const int		input,
	std::ifstream*		input2,
	JString*		tempFullName,
	const JBoolean	closeInput
	)
{
	JString data;
	if (!JReadAll(input, &data, closeInput))
		{
		return kJFalse;
		}

	if (!(JCreateTempFile(tempFullName)).OK())
		{
		return kJFalse;
		}

	std::ofstream output(tempFullName->GetBytes());
	data.Print(output);
	output.close();

	input2->open(tempFullName->GetBytes());
	return JI2B(input2->good());
}

#if 0

// These functions cannot be implemented with the latest draft specification of std::fstream

/******************************************************************************
 JOpenScratchFile

	Open a temporary file and return it as an std::fstream.  Since we can't convince
	the filebuf to own the file, you must call JCloseScratchFile() to throw out
	the file properly.

 ******************************************************************************/

std::fstream*
JOpenScratchFile()
{
	FILE* tempFile = tmpfile();
	int fd = fileno(tempFile);
	assert( fd != EOF );

	std::fstream* theFile = jnew std::fstream(fd);
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
	std::fstream** theFile
	)
{
	(**theFile).close();
	jdelete *theFile;
	*theFile = NULL;
}

#endif
