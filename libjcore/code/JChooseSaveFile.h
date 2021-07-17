/******************************************************************************
 JChooseSaveFile.h

	Interface for the JChooseSaveFile class.

	Copyright (C) 1994-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JChooseSaveFile
#define _H_JChooseSaveFile

#include "JPtrArray-JString.h"

class JChooseSaveFile
{
public:

	JChooseSaveFile();

	virtual ~JChooseSaveFile();

	virtual bool ChooseFile(const JString& prompt,
								const JString& instructions,
								JString* fullName) = 0;
	virtual bool ChooseFile(const JString& prompt,
								const JString& instructions,
								const JString& origName,
								JString* fullName) = 0;
	virtual bool ChooseFiles(const JString& prompt,
								 const JString& instructions,
								 JPtrArray<JString>* fullNameList) = 0;

	virtual bool ChooseRPath(const JString& prompt,
								 const JString& instructions,
								 const JString& origPath,
								 JString* newPath) = 0;
	virtual bool ChooseRWPath(const JString& prompt,
								  const JString& instructions,
								  const JString& origPath,
								  JString* newPath) = 0;

	virtual bool SaveFile(const JString& prompt,
							  const JString& instructions,
							  const JString& originalName,
							  JString* newFullName) = 0;

private:

	// not allowed

	JChooseSaveFile(const JChooseSaveFile& source);
	const JChooseSaveFile& operator=(const JChooseSaveFile& source);
};

#endif
