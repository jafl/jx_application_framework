/******************************************************************************
 jFStreamUtil.h

	Interface for jFStreamUtil.cc

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jFStreamUtil
#define _H_jFStreamUtil

#include <jTypes.h>

class JString;

// for convenience

#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;

void		JReadFile(const JCharacter* fileName, JString* str);
void		JReadFile(ifstream& input, JString* str);
void		JReadFile(fstream& input, JString* str);

JSize		JGetFStreamLength(ifstream& theStream);
JSize		JGetFStreamLength(fstream& theStream);
fstream*	JSetFStreamLength(const char* fileName, fstream& originalStream,
							  const JSize newLength, const JFStreamOpenMode io_mode);

JBoolean	JConvertToStream(const int input, ifstream* input2, JString* tempFullName,
							 const JBoolean closeInput = kJTrue);

#endif
