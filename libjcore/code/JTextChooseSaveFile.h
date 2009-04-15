/******************************************************************************
 JTextChooseSaveFile.h

	Interface for the JTextChooseSaveFile class.

	Copyright © 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTextChooseSaveFile
#define _H_JTextChooseSaveFile

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JChooseSaveFile.h>

class JTextChooseSaveFile : public JChooseSaveFile
{
public:

	JTextChooseSaveFile();

	virtual ~JTextChooseSaveFile();

	virtual JBoolean ChooseFile(const JCharacter* prompt,
								const JCharacter* instructions,		// can be NULL
								JString* fullName);
	virtual JBoolean ChooseFile(const JCharacter* prompt,
								const JCharacter* instructions,		// can be NULL
								const JCharacter* origName,			// can be NULL
								JString* fullName);
	virtual JBoolean ChooseFiles(const JCharacter* prompt,
								 const JCharacter* instructions,	// can be NULL
								 JPtrArray<JString>* fullNameList);

	virtual JBoolean ChooseRPath(const JCharacter* prompt,
								 const JCharacter* instructions,	// can be NULL
								 const JCharacter* origPath,		// can be NULL
								 JString* newPath);
	virtual JBoolean ChooseRWPath(const JCharacter* prompt,
								  const JCharacter* instructions,	// can be NULL
								  const JCharacter* origPath,		// can be NULL
								  JString* newPath);

	virtual JBoolean SaveFile(const JCharacter* prompt,
							  const JCharacter* instructions,		// can be NULL
							  const JCharacter* originalName,
							  JString* newFullName);

private:

	JBoolean	GetPath(const JCharacter* prompt,
						const JCharacter* instructions,
						JString* newPath);
	JBoolean	DoSystemCommand(const JString& str) const;

	// not allowed

	JTextChooseSaveFile(const JTextChooseSaveFile& source);
	const JTextChooseSaveFile& operator=(const JTextChooseSaveFile& source);
};

#endif
