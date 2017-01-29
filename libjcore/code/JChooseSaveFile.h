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

	virtual JBoolean ChooseFile(const JString& prompt,
								const JString& instructions,
								JString* fullName) = 0;
	virtual JBoolean ChooseFile(const JString& prompt,
								const JString& instructions,
								const JString& origName,
								JString* fullName) = 0;
	virtual JBoolean ChooseFiles(const JString& prompt,
								 const JString& instructions,
								 JPtrArray<JString>* fullNameList) = 0;

	virtual JBoolean ChooseRPath(const JString& prompt,
								 const JString& instructions,
								 const JString& origPath,
								 JString* newPath) = 0;
	virtual JBoolean ChooseRWPath(const JString& prompt,
								  const JString& instructions,
								  const JString& origPath,
								  JString* newPath) = 0;

	virtual JBoolean SaveFile(const JString& prompt,
							  const JString& instructions,
							  const JString& originalName,
							  JString* newFullName) = 0;

private:

	// not allowed

	JChooseSaveFile(const JChooseSaveFile& source);
	const JChooseSaveFile& operator=(const JChooseSaveFile& source);
};

#endif
