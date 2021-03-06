/******************************************************************************
 jStreamUtil.h

	Interface for jStreamUtil.cc

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jStreamUtil
#define _H_jStreamUtil

#include <jTypes.h>

class JString;

void	JCopyBinaryData(std::istream& input, std::ostream& output, const JSize byteCount);

JString	JRead(std::istream& input, const JSize count);
JString	JReadUntil(std::istream& input, const JCharacter delimiter,
				   JBoolean* foundDelimiter = NULL);
JString	JReadUntilws(std::istream& input, JBoolean* foundws = NULL);
JString	JReadLine(std::istream& input, JBoolean* foundNewLine = NULL);
void	JReadAll(std::istream& input, JString* str);

JBoolean JReadUntil(std::istream& input, const JSize delimiterCount,
					const JCharacter* delimiters, JString* str,
					JCharacter* delimiter = NULL);

void JIgnoreUntil(std::istream& input, const JCharacter delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(std::istream& input, const JCharacter* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(std::istream& input, const JSize delimiterCount,
					  const JCharacter* delimiters, JCharacter* delimiter = NULL);

void JIgnoreLine(std::istream& input, JBoolean* foundNewLine = NULL);

void		JEncodeBase64(std::istream& input, std::ostream& output);
JBoolean	JDecodeBase64(std::istream& input, std::ostream& output);

// compensate for lack of features in io stream library

JString		JRead(const int input, const JSize count);
JString		JReadUntil(const int input, const JCharacter delimiter,
					   JBoolean* foundDelimiter = NULL);
JBoolean	JReadAll(const int input, JString* str,
					 const JBoolean closeInput = kJTrue);

JBoolean JReadUntil(const int input, const JSize delimiterCount,
					const JCharacter* delimiters, JString* str,
					JCharacter* delimiter = NULL);

void JIgnoreUntil(const int input, const JCharacter delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(const int input, const JCharacter* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(const int input, const JSize delimiterCount,
					  const JCharacter* delimiters, JCharacter* delimiter = NULL);

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
