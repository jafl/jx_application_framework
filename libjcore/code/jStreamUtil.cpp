/******************************************************************************
 jStreamUtil.cpp

	Useful functions for dealing with streams.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#include "jStreamUtil.h"
#include "JString.h"
#include "JUtf8ByteBuffer.h"
#include "jFileUtil.h"
#include "jFStreamUtil.h"
#include "jErrno.h"
#include <poll.h>
#include <string.h>
#include <limits.h>
#include "jAssert.h"

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

	auto* data = jnew JUtf8Byte[ chunkSize ];
	assert( data != nullptr );

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
	JReadUntil(input, 0, nullptr, str, &c);
}

/******************************************************************************
 JReadLine

	Read characters from the std::istream until newline ('\n', '\r', '\r\n') is
	reached. newline is read in and discarded.

	If foundNewLine is not nullptr, it tells whether or not the end of a line
	was actually encountered.

 ******************************************************************************/

JString
JReadLine
	(
	std::istream&	input,
	bool*		foundNewLine
	)
{
	JString line;
	JUtf8Byte c;
	const bool foundDelimiter = JReadUntil(input, 2, "\r\n", &line, &c);
	if (foundDelimiter && c == '\r' && !input.eof() && input.peek() == '\n')
	{
		input.ignore();
	}
	if (foundNewLine != nullptr)
	{
		*foundNewLine = foundDelimiter;
	}
	return line;
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not nullptr, it tells whether or not the delimiter
	was actually found.

 ******************************************************************************/

JString
JReadUntil
	(
	std::istream&	input,
	const JUtf8Byte	delimiter,
	bool*			foundDelimiter
	)
{
	JString str;
	JUtf8Byte c;
	const bool found = JReadUntil(input, 1, &delimiter, &str, &c);
	if (foundDelimiter != nullptr)
	{
		*foundDelimiter = found;
	}
	return str;
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns true if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns false if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be nullptr.

 ******************************************************************************/

bool
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
	bool isDelimiter = false;

	const JSize bufSize = 1024;
	JUtf8Byte buf[ bufSize ];
	JUtf8Byte* p = buf;

	JUtf8Character c;
	while (true)
	{
		input >> c;
		if (input.fail())
		{
			break;
		}

		for (JUnsignedOffset j=0; j<delimiterCount; j++)
		{
			if (c == delimiters[j])
			{
				isDelimiter = true;
				if (delimiter != nullptr)
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
		if (p + byteCount - buf >= (JInt64) bufSize)
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

	If foundws is not nullptr, it tells whether or not whitespace
	was actually encountered.

 ******************************************************************************/

JString
JReadUntilws
	(
	std::istream&	input,
	bool*		foundws
	)
{
	JUtf8Byte delimiters[] = { ' ', '\t', '\n' };
	const JSize delimiterCount = sizeof(delimiters)/sizeof(JUtf8Byte);

	JString str;
	JUtf8Byte c;
	const bool found = JReadUntil(input, delimiterCount, delimiters, &str, &c);
	if (foundws != nullptr)
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

	If foundNewLine is not nullptr, it tells whether or not the end of a line
	was actually encountered.

 ******************************************************************************/

void
JIgnoreLine
	(
	std::istream&	input,
	bool*		foundNewLine
	)
{
	JUtf8Byte c;
	const bool foundDelimiter = JIgnoreUntil(input, 2, "\r\n", &c);
	if (foundDelimiter && c == '\r' && !input.eof() && input.peek() == '\n')
	{
		input.ignore();
	}
	if (foundNewLine != nullptr)
	{
		*foundNewLine = foundDelimiter;
	}
}

/******************************************************************************
 JIgnoreUntil

	Discard characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not nullptr, it tells whether or not the delimiter
	was actually found.

	To keep the prototype as close to JReadUntil() as possible,
	foundDelimiter is not the return value.

 ******************************************************************************/

void
JIgnoreUntil
	(
	std::istream&	input,
	const JUtf8Byte	delimiter,
	bool*		foundDelimiter
	)
{
	JUtf8Byte c;
	const bool found = JIgnoreUntil(input, 1, &delimiter, &c);
	if (foundDelimiter != nullptr)
	{
		*foundDelimiter = found;
	}
}

void 
JIgnoreUntil
	(
	std::istream&		input, 
	const JUtf8Byte*	delimiter,
	bool*			foundDelimiter
	)
{
	const JSize delimLength = strlen(delimiter);
	assert( delimLength > 0 );

	if (delimLength == 1)
	{
		JIgnoreUntil(input, delimiter[0], foundDelimiter);
		return;
	}

	if (foundDelimiter != nullptr)
	{
		*foundDelimiter = false;
	}

	auto* window = jnew JUtf8Byte[ delimLength ];
	assert( window != nullptr );

	input.read(window, delimLength);

	while (!input.eof() && !input.fail() &&
		   JString::Compare(window, delimLength, delimiter, delimLength) != 0)
	{
		memmove(window, window+1, delimLength-1);
		window[ delimLength-1 ] = input.get();
	}

	if (foundDelimiter != nullptr &&
		JString::Compare(window, delimLength, delimiter, delimLength) == 0)
	{
		*foundDelimiter = true;
	}

	jdelete [] window;
}

/******************************************************************************
 JIgnoreUntil

	Toss characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns true if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns false if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be nullptr.

 ******************************************************************************/

bool
JIgnoreUntil
	(
	std::istream&		input,
	const JSize			delimiterCount,
	const JUtf8Byte*	delimiters,
	JUtf8Byte*			delimiter
	)
{
	bool isDelimiter = false;

	while (true)
	{
		JUtf8Byte c;
		input.get(c);
		if (input.fail())
		{
			break;
		}

		for (JUnsignedOffset i=0; i<delimiterCount; i++)
		{
			if (c == delimiters[i])
			{
				isDelimiter = true;
				if (delimiter != nullptr)
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
static bool kBase64DecodeInit = false;

bool
JDecodeBase64
	(
	std::istream& input,
	std::ostream& output
	)
{
	if (!kBase64DecodeInit)
	{
		memset(kBase64Decoding, 0xFF, sizeof(kBase64Decoding));

		for (JUnsignedOffset i=0; i<64; i++)
		{
			kBase64Decoding[ (unsigned char) kBase64Encoding[i] ] = i;
		}

		kBase64DecodeInit = true;
	}

	input >> std::ws;

	bool lastReturn = false;
	JIndex chunkIndex   = 1;
	JIndex chunkData    = 0;

	unsigned char c = input.get();
	while (input.good())
	{
		if (c == '\n')
		{
			if (lastReturn)
			{
				return true;
			}
			else
			{
				lastReturn = true;
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
				return true;
			}
			else if (chunkIndex == 4)
			{
				unsigned char outVal = chunkData >> 16;
				output << outVal;

				outVal = (chunkData >> 8) & 0x000000FF;
				output << outVal;

				JReadUntilws(input);
				return true;
			}

			chunkIndex++;
		}
		else if (c > 127 || kBase64Decoding[c] == 0xFFFFFFFF)
		{
			return false;
		}
		else
		{
			lastReturn      = false;	// prevent single \n from triggering exit
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

	return chunkIndex == 1;
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
	size_t &bytes_transferred = (bt == nullptr ? temp : *bt);
	ssize_t n;

	bytes_transferred = 0;
	while (bytes_transferred < len)
	{
		n = jRead(handle, (char *) buf + bytes_transferred, len - bytes_transferred);

		if (n == -1 || n == 0)
		{
			return n;
		}
		bytes_transferred += n;
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
	assert( buf != nullptr );

	size_t dataLength;
	jReadN(input, buf, count, &dataLength);

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

bool
JReadAll
	(
	const int	input,
	JString*	str,
	const bool	closeInput
	)
{
	str->Clear();

	const JSize bufLength = 1024;

	char readBuf[ bufLength ];
	JUtf8ByteBuffer byteBuf(bufLength);
	while (true)
	{
		size_t byteCount;
		const ssize_t result = jReadN(input, readBuf, bufLength, &byteCount);

		if (byteCount > 0)
		{
			byteBuf.Append(std::span(readBuf, byteCount));
		}

		if (result == -1)
		{
			if (closeInput)
			{
				::close(input);
			}
			str->Set(byteBuf.GetCArray(), byteBuf.GetElementCount());
			return false;
		}
		else if (result == 0)
		{
			if (closeInput)
			{
				::close(input);
			}
			str->Set(byteBuf.GetCArray(), byteBuf.GetElementCount());
			return true;
		}
		// else, keep reading
	}
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not nullptr, it tells whether or not the delimiter
	was actually found.

 ******************************************************************************/

JString
JReadUntil
	(
	const int		input,
	const JUtf8Byte	delimiter,
	bool*		foundDelimiter
	)
{
	JString str;
	JUtf8Byte c;
	const bool found = JReadUntil(input, 1, &delimiter, &str, &c);
	if (foundDelimiter != nullptr)
	{
		*foundDelimiter = found;
	}
	return str;
}

/******************************************************************************
 JReadUntil

	Read characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns true if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns false if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be nullptr.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

bool
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
	bool isDelimiter = false;

	const JSize bufLength = 1024;

	JUtf8ByteBuffer byteBuf(bufLength);
	while (true)
	{
		JUtf8Byte c;
		size_t dataLength;
		const ssize_t result = jReadN(input, &c, 1, &dataLength);

		if (result == -1 || result == 0)
		{
			break;
		}

		for (JUnsignedOffset j=0; j<delimiterCount; j++)
		{
			if (c == delimiters[j])
			{
				isDelimiter = true;
				if (delimiter != nullptr)
				{
					*delimiter = c;
				}
				goto done;
			}
		}

		byteBuf.Append(std::span(&c, 1));
	}
done:
	str->Set(byteBuf.GetCArray(), byteBuf.GetElementCount());
	return isDelimiter;
}

/******************************************************************************
 JIgnoreUntil

	Discard characters from the std::istream until delimiter is reached.
	delimiter is read in and discarded.

	If foundDelimiter is not nullptr, it tells whether or not the delimiter
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
	bool*			foundDelimiter
	)
{
	JUtf8Byte c;
	const bool found = JIgnoreUntil(input, 1, &delimiter, &c);
	if (foundDelimiter != nullptr)
	{
		*foundDelimiter = found;
	}
}

void 
JIgnoreUntil
	(
	const int			input, 
	const JUtf8Byte*	delimiter,
	bool*				foundDelimiter
	)
{
	const JSize delimLength = strlen(delimiter);
	assert( delimLength > 0 );

	if (delimLength == 1)
	{
		JIgnoreUntil(input, delimiter[0], foundDelimiter);
		return;
	}

	if (foundDelimiter != nullptr)
	{
		*foundDelimiter = false;
	}

	char* window = jnew char[ delimLength+1 ];
	assert( window != nullptr );
	window[ delimLength ] = 0;

	size_t dataLength;
	ssize_t result = jReadN(input, window, delimLength, &dataLength);

	while (result > 0 && dataLength > 0 && strcmp(window, delimiter) != 0)
	{
		memmove(window, window+1, delimLength-1);
		result = jReadN(input, window + delimLength-1, 1, &dataLength);
	}

	if (strcmp(window, delimiter) == 0 && foundDelimiter != nullptr)
	{
		*foundDelimiter = true;
	}

	jdelete [] window;
}

/******************************************************************************
 JIgnoreUntil

	Toss characters from the std::istream until one of the delimiters is reached.
	delimiter is read in and discarded.

	Returns true if a delimited is found.  *delimiter is then set to
	the delimiter that was found.

	Returns false if it encounters an error or end-of-stream instead of
	a delimiter.  *delimiter is not changed.

	delimiter can be nullptr.

	This would be unnecessary if libstdc++ provided a stream wrapper for
	arbitrary file descriptors.

 ******************************************************************************/

bool
JIgnoreUntil
	(
	const int			input,
	const JSize			delimiterCount,
	const JUtf8Byte*	delimiters,
	JUtf8Byte*			delimiter
	)
{
	bool isDelimiter = false;

	while (true)
	{
		char c;
		size_t dataLength;
		ssize_t result = jReadN(input, &c, 1, &dataLength);

		if (result == -1)
		{
			return false;
		}

		for (JUnsignedOffset i=0; i<delimiterCount; i++)
		{
			if (c == delimiters[i])
			{
				isDelimiter = true;
				if (delimiter != nullptr)
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

bool
JWaitForInput
	(
	const int		input,
	const time_t	timeout
	)
{
	const time_t startTime = time(nullptr);
	while (true)
	{
		jclear_errno();

		pollfd in;
		in.fd      = input;
		in.events  = POLLRDNORM;
		in.revents = 0;

		const int result = poll(&in, 1, 1000);
		if (result > 0)								// success
		{
			return true;
		}
		else if (result < 0 && jerrno() != EINTR)	// failure
		{
			return false;
		}

		if (time(nullptr) - startTime > timeout)			// give up
		{
			return false;
		}
	}
}
