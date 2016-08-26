/******************************************************************************
 jStreamUtil.h

	Interface for jStreamUtil.cc

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jStreamUtil
#define _H_jStreamUtil

#include <jTypes.h>

class JString;

void	JCopyBinaryData(istream& input, ostream& output, const JSize byteCount);

JString	JRead(istream& input, const JSize count);
JString	JReadUntil(istream& input, const JCharacter delimiter,
				   JBoolean* foundDelimiter = NULL);
JString	JReadUntilws(istream& input, JBoolean* foundws = NULL);
JString	JReadLine(istream& input, JBoolean* foundNewLine = NULL);
void	JReadAll(istream& input, JString* str);

JBoolean JReadUntil(istream& input, const JSize delimiterCount,
					const JCharacter* delimiters, JString* str,
					JCharacter* delimiter = NULL);

void JIgnoreUntil(istream& input, const JCharacter delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(istream& input, const JCharacter* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(istream& input, const JSize delimiterCount,
					  const JCharacter* delimiters, JCharacter* delimiter = NULL);

void JIgnoreLine(istream& input, JBoolean* foundNewLine = NULL);

void		JEncodeBase64(istream& input, ostream& output);
JBoolean	JDecodeBase64(istream& input, ostream& output);

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

JSize JTellg(istream& stream);
void  JSeekg(istream& stream, streampos position);
void  JSeekg(istream& stream, streamoff offset, JIOStreamSeekDir direction);

JSize JTellp(ostream& stream);
void  JSeekp(ostream& stream, streampos position);
void  JSeekp(ostream& stream, streamoff offset, JIOStreamSeekDir direction);

// compensate for Sun CC brain damage

void	JSetState(ios& stream, const int flag);

#endif
