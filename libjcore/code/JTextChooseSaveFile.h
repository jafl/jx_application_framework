/******************************************************************************
 JTextChooseSaveFile.h

	Interface for the JTextChooseSaveFile class.

	Copyright (C) 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTextChooseSaveFile
#define _H_JTextChooseSaveFile

#include <JChooseSaveFile.h>

class JTextChooseSaveFile : public JChooseSaveFile
{
public:

	JTextChooseSaveFile();

	virtual ~JTextChooseSaveFile();

	virtual JBoolean ChooseFile(const JUtf8Byte& prompt,
								const JString* instructions,		// can be NULL
								JString* fullName);
	virtual JBoolean ChooseFile(const JString& prompt,
								const JString* instructions,		// can be NULL
								const JString* origName,			// can be NULL
								JString* fullName);
	virtual JBoolean ChooseFiles(const JString& prompt,
								 const JString* instructions,	// can be NULL
								 JPtrArray<JString>* fullNameList);

	virtual JBoolean ChooseRPath(const JString& prompt,
								 const JString* instructions,	// can be NULL
								 const JString* origPath,		// can be NULL
								 JString* newPath);
	virtual JBoolean ChooseRWPath(const JString& prompt,
								  const JString* instructions,	// can be NULL
								  const JString* origPath,		// can be NULL
								  JString* newPath);

	virtual JBoolean SaveFile(const JString& prompt,
							  const JString* instructions,		// can be NULL
							  const JString& originalName,
							  JString* newFullName);

private:

	JBoolean	GetPath(const JString& prompt,
						const JString* instructions,
						JString* newPath);
	JBoolean	DoSystemCommand(const JString& str) const;

	// not allowed

	JTextChooseSaveFile(const JTextChooseSaveFile& source);
	const JTextChooseSaveFile& operator=(const JTextChooseSaveFile& source);
};

#endif
