/******************************************************************************
 jStreamUtil.cpp

	Useful functions for dealing with streams.

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jStreamUtil.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jErrno.h>
#include <poll.h>
#include <string.h>
#include <limits.h>
#include <jAssert.h>

/******************************************************************************
 JCopyBinaryData

	Copies binary data from input to output.
	Caller must set initial read and write marks.

 ******************************************************************************/

void
JCopyBinaryData
	(
	std::istream&	input,
	std::ostream&	output,
	const JSize		byteCount
	)
{
	JSize chunkSize = 65536;

	// allocate transfer space

	JUtf8Byte* data = jnew JUtf8Byte[ chunkSize ];
	assert( data != NULL );

	// copy the data in chunks

	JSize readCount = 0;
	while (!input.eof() && !input.fail() && readCount < byteCount)
		{
		// don't read past byteCount

		if (readCount + chunkSize > byteCount)
			{
			chunkSize = byteCount - readCount;
			}

		// transfer the chunk

		input.read(data, chunkSize);
		output.write(data, chunkSize);
		readCount += chunkSize;
		}

	jdelete [] data;

	assert( !input.bad() );

	// since nothing failed, we don't want to leave the eof bit set

	input.clear();
}

/******************************************************************************
 JRead

	Read the specified number of characters from the stream.

 ******************************************************************************/

JString
JRead
	(
	std::istream&	input,
	const JSize		count
	)
{
	JString str;
	str.Read(input, count);
	return str;
}

/******************************************************************************
 JReadAll

	Read characters until the end of the std::istream is reached.
	This function takes a JString* because the contents of the stream
	could be very large, and returning a JString requires twice as much
	memory because of the copy constructor.

	Not inline to avoid including JString.h in header file.

 ******************************************************************************/

void
JReadAll
	(
	std::istream&	input,
	JString*		str
	)
{
	JUtf8Byte c;
	JReadUntil(input, 0, NULL, str, &c);
}

/******************************************************************************
 JReadLine

	Read characters from the std::istream until newline ('\n', '\r', '\r\n') is
	reached. newline is read in and discarded.

	If foundNewLine is not NULL, it tells whether or not the end of a line
	was actually encountered.

 ******************************************************************************/

JString
JReadLine
	(
	std::istream&	input,
	JBoolean*		foundNewLine
	)
{
	JString line;
	JUtf8Byte c;
	const JBoolean foundDelimiter = JReadUntil(input, 2, "\r\n", &line, &c);
	if (foundDelimiter && c == '\r' && !input.eof() && input.peek() == '\n')
		{
		input.ignore();
		}
	if (foundNewLine != NULL)
		{
		*foundNewLine = foundDelimiter;
		}
	return line;
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

 ******************************************************************************/

JString
JReadUntil
	(
	std::istream&	input,
	const JUtf8Byte	delimiter,
	JBoolean*		foundDelimiter
	)
{
	JString str;
	JUtf8Byte c;
	const JBoolean found = JReadUntil(input, 1, &delimiter, &str, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
	return str;
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

 ******************************************************************************/

JBoolean
JReadUntil
	(
	std::istream&		input,
	const JSize			delimiterCount,
	const JUtf8Byte*	delimiters,
	JString*			str,
	JUtf8Byte*			delimiter
	)
{
	str->Clear();
	JBoolean isDelimiter = kJFalse;

	const JSize bufSize = 1024;
	JUtf8Byte buf[ bufSize ];
	JUtf8Byte* p = buf;

	JUtf8Character c;
	while (1)
		{
		input >> c;
		if (input.fail())
			{
			break;
			}

		for (JIndex j=0; j<delimiterCount; j++)
			{
			if (c == delimiters[j])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = delimiters[j];
					}
				break;
				}
			}

		if (isDelimiter || input.eof())
			{
			break;
			}

		const JSize byteCount = c.GetByteCount();
		if (p + byteCount - buf >= bufSize)
			{
			str->Append(buf, p - buf);
			p = buf;
			}

		memcpy(p, c.GetBytes(), byteCount);
		p += c.GetByteCount();
		}

	if (p > buf)
		{
		str->Append(buf, p - buf);
		}
	return isDelimiter;
}

/******************************************************************************
 JReadUntilws

	Read characters from the std::istream until white-space is reached.
	white-space is read in and discarded.

	If foundws is not NULL, it tells whether or not whitespace
	was actually encountered.

 ******************************************************************************/

JString
JReadUntilws
	(
	std::istream&	input,
	JBoolean*		foundws
	)
{
	JUtf8Byte delimiters[] = { ' ', '\t', '\n' };
	const JSize delimiterCount = sizeof(delimiters)/sizeof(JUtf8Byte);

	JString str;
	JUtf8Byte c;
	const JBoolean found = JReadUntil(input, delimiterCount, delimiters, &str, &c);
	if (foundws != NULL)
		{
		*foundws = found;
		}
	input >> std::ws;
	return str;
}

/******************************************************************************
 JIgnoreLine

	Toss characters from the std::istream until newline ('\n', '\r', '\r\n') is
	reached. newline is read in and discarded.

	If foundNewLine is not NULL, it tells whether or not the end of a line
	was actually encountered.

 ******************************************************************************/

void
JIgnoreLine
	(
	std::istream&	input,
	JBoolean*		foundNewLine
	)
{
	JUtf8Byte c;
	const JBoolean foundDelimiter = JIgnoreUntil(input, 2, "\r\n", &c);
	if (foundDelimiter && c == '\r' && !input.eof() && input.peek() == '\n')
		{
		input.ignore();
		}
	if (foundNewLine != NULL)
		{
		*foundNewLine = foundDelimiter;
		}
}

/******************************************************************************
 JIgnoreUntil

	Discard characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

	To keep the prototype as close to JReadUntil() as possible,
	foundDelimiter is not the return value.

 ******************************************************************************/

void
JIgnoreUntil
	(
	std::istream&	input,
	const JUtf8Byte	delimiter,
	JBoolean*		foundDelimiter
	)
{
	JUtf8Byte c;
	const JBoolean found = JIgnoreUntil(input, 1, &delimiter, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
}

void 
JIgnoreUntil
	(
	std::istream&		input, 
	const JUtf8Byte*	delimiter,
	JBoolean*			foundDelimiter
	)
{
	const JSize delimLength = strlen(delimiter);
	assert( delimLength > 0 );

	if (delimLength == 1)
		{
		JIgnoreUntil(input, delimiter[0], foundDelimiter);
		return;
		}

	if (foundDelimiter != NULL)
		{
		*foundDelimiter = kJFalse;
		}

	JUtf8Byte* window = jnew JUtf8Byte[ delimLength ];
	assert( window != NULL );

	input.read(window, delimLength);

	while (!input.eof() && !input.fail() &&
		   JString::Compare(window, delimLength, delimiter, delimLength) != 0)
		{
		memmove(window, window+1, delimLength-1);
		window[ delimLength-1 ] = input.get();
		}

	if (foundDelimiter != NULL &&
		JString::Compare(window, delimLength, delimiter, delimLength) == 0)
		{
		*foundDelimiter = kJTrue;
		}

	jdelete [] window;
}

/******************************************************************************
 JIgnoreUntil

	Toss characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

 ******************************************************************************/

JBoolean
JIgnoreUntil
	(
	std::istream&		input,
	const JSize			delimiterCount,
	const JUtf8Byte*	delimiters,
	JUtf8Byte*			delimiter
	)
{
	JBoolean isDelimiter = kJFalse;

	while (1)
		{
		JUtf8Byte c;
		input.get(c);
		if (input.fail())
			{
			break;
			}

		for (JIndex i=0; i<delimiterCount; i++)
			{
			if (c == delimiters[i])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || input.eof())
			{
			break;
			}
		}

	return isDelimiter;
}

/******************************************************************************
 JEncodeBase64

	Reads data from the input stream and writes the base64 encoded version
	to the ouput stream.  This was chosen as the bottom-layer function
	because the data may be too large to load into memory.

 ******************************************************************************/

static const JUtf8Byte kBase64Encoding[] =
	{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
	};

static const JIndex kMaxCharOnLine = 72;	// short enough to allow == pad to ignore line length check

static void
jWriteBase64Byte
	(
	std::ostream&		output,
	const JIndex	bIndex,
	JIndex*			cIndex
	)
{
	output << kBase64Encoding[bIndex];

	*cIndex = *cIndex + 1;
	if (*cIndex > kMaxCharOnLine)
		{
		output << "\r\n";
		*cIndex = 1;
		}
}

void
JEncodeBase64
	(
	std::istream& input,
	std::ostream& output
	)
{
	JIndex chunkIndex = 1;
	JIndex chunkData  = 0;
	JIndex charIndex  = 1;

	unsigned char c = input.get();
	while (input.good())
		{
		JIndex pieceVal = c;
		if (chunkIndex == 1)
			{
			chunkData = pieceVal << 16;
			}
		else if (chunkIndex == 2)
			{
			chunkData = chunkData | (pieceVal << 8);
			}
		else if (chunkIndex == 3)
			{
			chunkData = chunkData | pieceVal;
			JIndex b64Index = (chunkData >> 18) & 0x0000003F;
			jWriteBase64Byte(output, b64Index, &charIndex);

			b64Index = (chunkData >> 12) & 0x0000003F;
			jWriteBase64Byte(output, b64Index, &charIndex);

			b64Index = (chunkData >> 6) & 0x0000003F;
			jWriteBase64Byte(output, b64Index, &charIndex);

			b64Index = chunkData & 0x0000003F;
			jWriteBase64Byte(output, b64Index, &charIndex);

			chunkData = 0;
			}

		c = input.get();
		chunkIndex++;
		if (chunkIndex > 3)
			{
			chunkIndex = 1;
			}
		}

	// write final bytes

	if (chunkIndex == 2)
		{
		JIndex b64Index = chunkData >> 18;
		jWriteBase64Byte(output, b64Index, &charIndex);

		b64Index = (chunkData >> 12) & 0x0000003F;
		jWriteBase64Byte(output, b64Index, &charIndex);

		output << "==";
		}
	else if (chunkIndex == 3)
		{
		JIndex b64Index = chunkData >> 18;
		jWriteBase64Byte(output, b64Index, &charIndex);

		b64Index = (chunkData >> 12) & 0x0000003F;
		jWriteBase64Byte(output, b64Index, &charIndex);

		b64Index = (chunkData >> 6) & 0x0000003F;
		jWriteBase64Byte(output, b64Index, &charIndex);

		output << '=';
		}
	output << "\r\n";
}

/******************************************************************************
 JDecodeBase64

	Reads base64-encoded data from the input stream and writes the original
	version to the ouput stream.  This was chosen as the bottom-layer
	function because the data may be too large to load into memory.

 ******************************************************************************/

static JIndex kBase64Decoding[128];
static JBoolean kBase64DecodeInit = kJFalse;

JBoolean
JDecodeBase64
	(
	std::istream& input,
	std::ostream& output
	)
{
	if (!kBase64DecodeInit)
		{
		memset(kBase64Decoding, 0xFF, sizeof(kBase64Decoding));

		for (JIndex i=0; i<64; i++)
			{
			kBase64Decoding[ (unsigned char) kBase64Encoding[i] ] = i;
			}

		kBase64DecodeInit = kJTrue;
		}

	input >> std::ws;

	JBoolean lastReturn = kJFalse;
	JIndex chunkIndex   = 1;
	JIndex chunkData    = 0;

	unsigned char c = input.get();
	while (input.good())
		{
		if (c == '\n')
			{
			if (lastReturn)
				{
				return kJTrue;
				}
			else
				{
				lastReturn = kJTrue;
				}
			}
		else if (c == '\r')
			{
			// skip it
			}
		else if (c == '=')
			{
			if (chunkIndex == 3)
				{
				unsigned char outVal = chunkData >> 16;
				output << outVal;

				JReadUntilws(input);
				return kJTrue;
				}
			else if (chunkIndex == 4)
				{
				unsigned char outVal = chunkData >> 16;
				output << outVal;

				outVal = (chunkData >> 8) & 0x000000FF;
				output << outVal;

				JReadUntilws(input);
				return kJTrue;
				}

			chunkIndex++;
			}
		else if (c > 127 || kBase64Decoding[c] == 0xFFFFFFFF)
			{
			return kJFalse;
			}
		else
			{
			lastReturn      = kJFalse;	// prevent single \n from triggering exit
			JIndex pieceVal = kBase64Decoding[c];

			if (chunkIndex == 1)
				{
				pieceVal  = pieceVal << 18;
				chunkData = chunkData | pieceVal;
				}
			else if (chunkIndex == 2)
				{
				pieceVal  = pieceVal << 12;
				chunkData = chunkData | pieceVal;
				}
			else if (chunkIndex == 3)
				{
				pieceVal  = pieceVal << 6;
				chunkData = chunkData | pieceVal;
				}
			else if (chunkIndex == 4)
				{
				chunkData = chunkData | pieceVal;

				unsigned char outVal = chunkData >> 16;
				output << outVal;

				outVal = (chunkData >> 8) & 0x000000FF;
				output << outVal;

				outVal = chunkData & 0x000000FF;
				output << outVal;

				chunkData = 0;
				}

			chunkIndex++;
			}

		c = input.get();
		if (chunkIndex > 4)
			{
			chunkIndex = 1;
			}
		}

	return JI2B(chunkIndex == 1);
}

/******************************************************************************
 jReadN

	Stolen from ACE library because we don't want to use ACE_HANDLE.  This
	is a HANDLE instead of an int on Windows.

 ******************************************************************************/

inline ssize_t
jRead
	(
	const int	handle,
	void*		buf,
	size_t		len
	)
{
	# if defined (ACE_HAS_CHARPTR_SOCKOPT)
	return (ssize_t) ::read(handle, (char *) buf, len);
	# else
	return (ssize_t) ::read(handle, buf, len);
	# endif /* ACE_HAS_CHARPTR_SOCKOPT */
}

ssize_t
jReadN
	(
	const int	handle,
	void*		buf,
	size_t		len,
	size_t*		bt
	)
{
	size_t temp;
	size_t &bytes_transferred = (bt == 0 ? temp : *bt);
	ssize_t n;

	for (bytes_transferred = 0;
		 bytes_transferred < len;
		 bytes_transferred += n)
		{
		n = jRead(handle, (char *) buf + bytes_transferred, len - bytes_transferred);

		if (n == -1 || n == 0)
			{
			return n;
			}
		}

	return bytes_transferred;
}

/******************************************************************************
 JRead

	Read the specified number of characters from the stream.

 ******************************************************************************/

JString
JRead
	(
	const int	input,
	const JSize	count
	)
{
	char* buf = jnew char[ count ];
	assert( buf != NULL );

	size_t dataLength;
	ssize_t result = jReadN(input, buf, count, &dataLength);

	JString str(buf, dataLength);

	jdelete [] buf;

	return str;
}

/******************************************************************************
 JReadAll

	Read characters until the end of the data stream is reached.
	This function takes a JString* because the contents of the stream
	could be very large, and returning a JString requires twice as much
	memory because of the copy constructor.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JReadAll
	(
	const int		input,
	JString*		str,
	const JBoolean	closeInput
	)
{
	str->Clear();

	const JSize bufLength = 1024;

	char buf[ bufLength ];
	while (1)
		{
		size_t dataLength;
		ssize_t result = jReadN(input, buf, bufLength, &dataLength);

		if (dataLength > 0)
			{
			str->Append(buf, dataLength);	// TODO: append only complete characters
			}

		if (result == -1)
			{
			if (closeInput)
				{
				::close(input);
				}
			return kJFalse;
			}
		else if (result == 0)
			{
			if (closeInput)
				{
				::close(input);
				}
			return kJTrue;
			}
		// else, keep reading
		}
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

 ******************************************************************************/

JString
JReadUntil
	(
	const int		input,
	const JUtf8Byte	delimiter,
	JBoolean*		foundDelimiter
	)
{
	JString str;
	JUtf8Byte c;
	const JBoolean found = JReadUntil(input, 1, &delimiter, &str, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
	return str;
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JReadUntil
	(
	const int			input,
	const JSize			delimiterCount,
	const JUtf8Byte*	delimiters,
	JString*			str,
	JUtf8Byte*			delimiter
	)
{
	str->Clear();
	JBoolean isDelimiter = kJFalse;

	const JSize bufSize = 1024;
	JUtf8Byte buf[ bufSize ];

	JIndex i = 0;
	while (1)
		{
		char c;
		size_t dataLength;
		ssize_t result = jReadN(input, &c, 1, &dataLength);

		if (result == -1)
			{
			return kJFalse;
			}

		for (JIndex j=0; j<delimiterCount; j++)
			{
			if (c == delimiters[j])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || result == 0)
			{
			break;
			}
		else
			{
			buf[i] = c;
			i++;
			if (i == bufSize)
				{
				str->Append(buf, bufSize);	// TODO: append only complete characters
				i=0;
				}
			}
		}

	str->Append(buf, i);
	return isDelimiter;
}

/******************************************************************************
 JIgnoreUntil

	Discard characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not NULL, it tells whether or not the delimiter
	was actually found.

	To keep the prototype as close to JReadUntil() as possible,
	foundDelimiter is not the return value.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

void
JIgnoreUntil
	(
	const int		input,
	const JUtf8Byte	delimiter,
	JBoolean*		foundDelimiter
	)
{
	JUtf8Byte c;
	const JBoolean found = JIgnoreUntil(input, 1, &delimiter, &c);
	if (foundDelimiter != NULL)
		{
		*foundDelimiter = found;
		}
}

void 
JIgnoreUntil
	(
	const int			input, 
	const JUtf8Byte*	delimiter,
	JBoolean*			foundDelimiter
	)
{
	const JSize delimLength = strlen(delimiter);
	assert( delimLength > 0 );

	if (delimLength == 1)
		{
		JIgnoreUntil(input, delimiter[0], foundDelimiter);
		return;
		}

	if (foundDelimiter != NULL)
		{
		*foundDelimiter = kJFalse;
		}

	char* window = jnew char[ delimLength ];
	assert( window != NULL );

	size_t dataLength;
	ssize_t result = jReadN(input, window, delimLength, &dataLength);

	while (result > 0 && dataLength > 0 && strcmp(window, delimiter) != 0)
		{
		memmove(window, window+1, delimLength-1);
		result = jReadN(input, window + delimLength-1, 1, &dataLength);
		}

	if (strcmp(window, delimiter) == 0 && foundDelimiter != NULL)
		{
		*foundDelimiter = kJTrue;
		}

	jdelete [] window;
}

/******************************************************************************
 JIgnoreUntil

	Toss characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns kJTrue if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns kJFalse if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be NULL.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

JBoolean
JIgnoreUntil
	(
	const int			input,
	const JSize			delimiterCount,
	const JUtf8Byte*	delimiters,
	JUtf8Byte*			delimiter
	)
{
	JBoolean isDelimiter = kJFalse;

	while (1)
		{
		char c;
		size_t dataLength;
		ssize_t result = jReadN(input, &c, 1, &dataLength);

		if (result == -1)
			{
			return kJFalse;
			}

		for (JIndex i=0; i<delimiterCount; i++)
			{
			if (c == delimiters[i])
				{
				isDelimiter = kJTrue;
				if (delimiter != NULL)
					{
					*delimiter = c;
					}
				break;
				}
			}

		if (isDelimiter || result == 0)
			{
			break;
			}
		}

	return isDelimiter;
}

/******************************************************************************
 JWaitForInput

	Wait until there is data available or until it times out.  timeout is
	in seconds.

 ******************************************************************************/

JBoolean
JWaitForInput
	(
	const int		input,
	const time_t	timeout
	)
{
	const time_t startTime = time(NULL);
	while (1)
		{
		jclear_errno();

		pollfd in;
		in.fd      = input;
		in.events  = POLLRDNORM;
		in.revents = 0;

		const int result = poll(&in, 1, 1000);
		if (result > 0)								// success
			{
			return kJTrue;
			}
		else if (result < 0 && jerrno() != EINTR)	// failure
			{
			return kJFalse;
			}

		if (time(NULL) - startTime > timeout)			// give up
			{
			return kJFalse;
			}
		}
}
