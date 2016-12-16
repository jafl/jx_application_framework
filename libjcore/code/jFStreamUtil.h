/******************************************************************************
 jFStreamUtil.h

	Interface for jFStreamUtil.cc

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jFStreamUtil
#define _H_jFStreamUtil

#include <jTypes.h>
#include <fstream> // templates

class JString;

void	JReadFile(const JString& fileName, JString* str);
void	JReadFile(std::ifstream& input, JString* str);
void	JReadFile(std::fstream& input, JString* str);

JSize			JGetFStreamLength(std::ifstream& theStream);
JSize			JGetFStreamLength(std::fstream& theStream);
std::fstream*	JSetFStreamLength(const char* fileName, std::fstream& originalStream,
								  const JSize newLength, const JFStreamOpenMode io_mode);

void		JReadFile(const JString& fileName, JString* str);
void		JReadFile(std::ifstream& input, JString* str);
void		JReadFile(std::fstream& input, JString* str);

JSize			JGetFStreamLength(std::ifstream& theStream);
JSize			JGetFStreamLength(std::fstream& theStream);
std::fstream*	JSetFStreamLength(const JString& fileName, std::fstream& originalStream,
								  const JSize newLength, const JFStreamOpenMode io_mode);

JBoolean	JConvertToStream(const int input, std::ifstream* input2, JString* tempFullName,
							 const JBoolean closeInput = kJTrue);

#endif
