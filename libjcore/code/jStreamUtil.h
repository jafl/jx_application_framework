/******************************************************************************
 jStreamUtil.h

	Interface for jStreamUtil.cc

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jStreamUtil
#define _H_jStreamUtil

#include <jTypes.h>

class JString;

void	JCopyBinaryData(istream& input, ostream& output, const JSize byteCount);

JString	JRead(istream& input, const JSize count);
JString	JReadUntil(istream& input, const JUtf8Byte delimiter,
				   JBoolean* foundDelimiter = NULL);
JString	JReadUntilws(istream& input, JBoolean* foundws = NULL);
JString	JReadLine(istream& input, JBoolean* foundNewLine = NULL);
void	JReadAll(istream& input, JString* str);

JBoolean JReadUntil(istream& input, const JSize delimiterCount,
					const JUtf8Byte* delimiters, JString* str,
					JUtf8Byte* delimiter = NULL);

void JIgnoreUntil(istream& input, const JUtf8Byte delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(istream& input, const JUtf8Byte* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(istream& input, const JSize delimiterCount,
					  const JUtf8Byte* delimiters, JUtf8Byte* delimiter = NULL);

void JIgnoreLine(istream& input, JBoolean* foundNewLine = NULL);

void		JEncodeBase64(istream& input, ostream& output);
JBoolean	JDecodeBase64(istream& input, ostream& output);

// compensate for lack of features in io stream library

JString		JRead(const int input, const JSize count);
JString		JReadUntil(const int input, const JUtf8Byte delimiter,
					   JBoolean* foundDelimiter = NULL);
JBoolean	JReadAll(const int input, JString* str,
					 const JBoolean closeInput = kJTrue);

JBoolean JReadUntil(const int input, const JSize delimiterCount,
					const JUtf8Byte* delimiters, JString* str,
					JUtf8Byte* delimiter = NULL);

void JIgnoreUntil(const int input, const JUtf8Byte delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(const int input, const JUtf8Byte* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(const int input, const JSize delimiterCount,
					  const JUtf8Byte* delimiters, JUtf8Byte* delimiter = NULL);

JBoolean	JWaitForInput(const int input, const time_t timeout);

// compensate for ANSI's removal of features

JSize JTellg(istream& stream);
void  JSeekg(istream& stream, streampos position);
void  JSeekg(istream& stream, streamoff offset, JIOStreamSeekDir direction);

JSize JTellp(ostream& stream);
void  JSeekp(ostream& stream, streampos position);
void  JSeekp(ostream& stream, streamoff offset, JIOStreamSeekDir direction);

// compensate for Sun CC brain damage

void	JSetState(ios& stream, const int flag);

#endif
