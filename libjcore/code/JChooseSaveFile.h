/******************************************************************************
 JChooseSaveFile.h

	Interface for the JChooseSaveFile class.

	Copyright © 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JChooseSaveFile
#define _H_JChooseSaveFile

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>

class JChooseSaveFile
{
public:

	JChooseSaveFile();

	virtual ~JChooseSaveFile();

	virtual JBoolean ChooseFile(const JCharacter* prompt,
								const JCharacter* instructions,		// can be NULL
								JString* fullName) = 0;
	virtual JBoolean ChooseFile(const JCharacter* prompt,
								const JCharacter* instructions,		// can be NULL
								const JCharacter* origName,			// can be NULL
								JString* fullName) = 0;
	virtual JBoolean ChooseFiles(const JCharacter* prompt,
								 const JCharacter* instructions,	// can be NULL
								 JPtrArray<JString>* fullNameList) = 0;

	virtual JBoolean ChooseRPath(const JCharacter* prompt,
								 const JCharacter* instructions,	// can be NULL
								 const JCharacter* origPath,		// can be NULL
								 JString* newPath) = 0;
	virtual JBoolean ChooseRWPath(const JCharacter* prompt,
								  const JCharacter* instructions,	// can be NULL
								  const JCharacter* origPath,		// can be NULL
								  JString* newPath) = 0;

	virtual JBoolean SaveFile(const JCharacter* prompt,
							  const JCharacter* instructions,		// can be NULL
							  const JCharacter* originalName,
							  JString* newFullName) = 0;

private:

	// not allowed

	JChooseSaveFile(const JChooseSaveFile& source);
	const JChooseSaveFile& operator=(const JChooseSaveFile& source);
};

#endif
