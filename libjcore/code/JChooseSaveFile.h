/******************************************************************************
 JChooseSaveFile.h

	Interface for the JChooseSaveFile class.

	Copyright (C) 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JChooseSaveFile
#define _H_JChooseSaveFile

#include <JPtrArray-JString.h>

class JChooseSaveFile
{
public:

	JChooseSaveFile();

	virtual ~JChooseSaveFile();

	virtual JBoolean ChooseFile(const JUtf8Byte* prompt,
								const JUtf8Byte* instructions,		// can be NULL
								JString* fullName) = 0;
	virtual JBoolean ChooseFile(const JUtf8Byte* prompt,
								const JUtf8Byte* instructions,		// can be NULL
								const JUtf8Byte* origName,			// can be NULL
								JString* fullName) = 0;
	virtual JBoolean ChooseFiles(const JUtf8Byte* prompt,
								 const JUtf8Byte* instructions,		// can be NULL
								 JPtrArray<JString>* fullNameList) = 0;

	virtual JBoolean ChooseRPath(const JUtf8Byte* prompt,
								 const JUtf8Byte* instructions,		// can be NULL
								 const JUtf8Byte* origPath,			// can be NULL
								 JString* newPath) = 0;
	virtual JBoolean ChooseRWPath(const JUtf8Byte* prompt,
								  const JUtf8Byte* instructions,	// can be NULL
								  const JUtf8Byte* origPath,		// can be NULL
								  JString* newPath) = 0;

	virtual JBoolean SaveFile(const JUtf8Byte* prompt,
							  const JUtf8Byte* instructions,		// can be NULL
							  const JUtf8Byte* originalName,
							  JString* newFullName) = 0;

private:

	// not allowed

	JChooseSaveFile(const JChooseSaveFile& source);
	const JChooseSaveFile& operator=(const JChooseSaveFile& source);
};

#endif
