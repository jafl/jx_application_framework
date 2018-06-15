/******************************************************************************
 jStreamUtil.h

	Interface for jStreamUtil.cc

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_jStreamUtil
#define _H_jStreamUtil

#include "jTypes.h"

class JString;

void	JCopyBinaryData(std::istream& input, std::ostream& output, const JSize byteCount);

JString	JRead(std::istream& input, const JSize count);
JString	JReadUntil(std::istream& input, const JUtf8Byte delimiter,
				   JBoolean* foundDelimiter = nullptr);
JString	JReadUntilws(std::istream& input, JBoolean* foundws = nullptr);
JString	JReadLine(std::istream& input, JBoolean* foundNewLine = nullptr);
void	JReadAll(std::istream& input, JString* str);

JBoolean JReadUntil(std::istream& input, const JSize delimiterCount,
					const JUtf8Byte* delimiters, JString* str,
					JUtf8Byte* delimiter = nullptr);

void JIgnoreUntil(std::istream& input, const JUtf8Byte delimiter,
				  JBoolean* foundDelimiter = nullptr);
void JIgnoreUntil(std::istream& input, const JUtf8Byte* delimiter,
				  JBoolean* foundDelimiter = nullptr);

JBoolean JIgnoreUntil(std::istream& input, const JSize delimiterCount,
					  const JUtf8Byte* delimiters, JUtf8Byte* delimiter = nullptr);

void JIgnoreLine(std::istream& input, JBoolean* foundNewLine = nullptr);

void		JEncodeBase64(std::istream& input, std::ostream& output);
JBoolean	JDecodeBase64(std::istream& input, std::ostream& output);

// compensate for lack of features in io stream library

JString		JRead(const int input, const JSize count);
JString		JReadUntil(const int input, const JUtf8Byte delimiter,
					   JBoolean* foundDelimiter = nullptr);
JBoolean	JReadAll(const int input, JString* str,
					 const JBoolean closeInput = kJTrue);

JBoolean JReadUntil(const int input, const JSize delimiterCount,
					const JUtf8Byte* delimiters, JString* str,
					JUtf8Byte* delimiter = nullptr);

void JIgnoreUntil(const int input, const JUtf8Byte delimiter,
				  JBoolean* foundDelimiter = nullptr);
void JIgnoreUntil(const int input, const JUtf8Byte* delimiter,
				  JBoolean* foundDelimiter = nullptr);

JBoolean JIgnoreUntil(const int input, const JSize delimiterCount,
					  const JUtf8Byte* delimiters, JUtf8Byte* delimiter = nullptr);

JBoolean	JWaitForInput(const int input, const time_t timeout);

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
