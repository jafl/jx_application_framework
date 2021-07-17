/******************************************************************************
 jStreamUtil.h

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_jStreamUtil
#define _H_jStreamUtil

#include "jTypes.h"

class JString;

void	JCopyBinaryData(std::istream& input, std::ostream& output, const JSize byteCount);

JString	JRead(std::istream& input, const JSize count);
JString	JReadUntil(std::istream& input, const JUtf8Byte delimiter,
				   bool* foundDelimiter = nullptr);
JString	JReadUntilws(std::istream& input, bool* foundws = nullptr);
JString	JReadLine(std::istream& input, bool* foundNewLine = nullptr);
void	JReadAll(std::istream& input, JString* str);

bool JReadUntil(std::istream& input, const JSize delimiterCount,
				const JUtf8Byte* delimiters, JString* str,
				JUtf8Byte* delimiter = nullptr);

void JIgnoreUntil(std::istream& input, const JUtf8Byte delimiter,
				  bool* foundDelimiter = nullptr);
void JIgnoreUntil(std::istream& input, const JUtf8Byte* delimiter,
				  bool* foundDelimiter = nullptr);

bool JIgnoreUntil(std::istream& input, const JSize delimiterCount,
				  const JUtf8Byte* delimiters, JUtf8Byte* delimiter = nullptr);

void JIgnoreLine(std::istream& input, bool* foundNewLine = nullptr);

void	JEncodeBase64(std::istream& input, std::ostream& output);
bool	JDecodeBase64(std::istream& input, std::ostream& output);

// compensate for lack of features in iostream library

JString		JRead(const int input, const JSize count);
JString		JReadUntil(const int input, const JUtf8Byte delimiter,
					   bool* foundDelimiter = nullptr);
bool		JReadAll(const int input, JString* str,
					 const bool closeInput = true);

bool JReadUntil(const int input, const JSize delimiterCount,
				const JUtf8Byte* delimiters, JString* str,
				JUtf8Byte* delimiter = nullptr);

void JIgnoreUntil(const int input, const JUtf8Byte delimiter,
				  bool* foundDelimiter = nullptr);
void JIgnoreUntil(const int input, const JUtf8Byte* delimiter,
				  bool* foundDelimiter = nullptr);

bool JIgnoreUntil(const int input, const JSize delimiterCount,
				  const JUtf8Byte* delimiters, JUtf8Byte* delimiter = nullptr);

bool	JWaitForInput(const int input, const time_t timeout);

// https://gist.github.com/andreasxp/ac9adcf8a2b37ac05ff7047f8728b3c7

class charbuf : public std::streambuf
{
public:

	charbuf
		(
		const char*			s,
		const std::size_t	count
		)
	{
		char* p = const_cast<char*>(s);
		this->setg(p, p, p + count);
	}

protected:

	virtual pos_type seekoff
		(
		off_type				off,
		std::ios_base::seekdir	dir,
		std::ios_base::openmode	which = std::ios_base::in | std::ios_base::out
		)
		override
	{
		if (dir == std::ios_base::cur)
			{
			gbump(off);
			}
		else if (dir == std::ios_base::end)
			{
			setg(eback(), egptr() + off, egptr());
			}
		else if (dir == std::ios_base::beg)
			{
			setg(eback(), eback() + off, egptr());
			}

		return gptr() - eback();
	}

	virtual pos_type seekpos
		(
		pos_type				sp,
		std::ios_base::openmode	which
		)
		override
	{
		return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
	}
};

class icharbufstream : private virtual charbuf, public std::istream
{
public:

	icharbufstream
		(
		const char*			s,
		const std::size_t	count
		)
		:
		charbuf(s, count), 
		std::istream(static_cast<std::streambuf*>(this))
	{ }
};

// compensate for ANSI's removal of features

JSize JTellg(std::istream& stream);
void  JSeekg(std::istream& stream, std::streampos position);
void  JSeekg(std::istream& stream, std::streamoff offset, JIOStreamSeekDir direction);

JSize JTellp(std::ostream& stream);
void  JSeekp(std::ostream& stream, std::streampos position);
void  JSeekp(std::ostream& stream, std::streamoff offset, JIOStreamSeekDir direction);

// compensate for Sun CC brain damage

void	JSetState(std::ios& stream, const int flag);

#endif
