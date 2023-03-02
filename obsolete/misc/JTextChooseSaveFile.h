/******************************************************************************
 JTextChooseSaveFile.h

	Interface for the JTextChooseSaveFile class.

	Copyright (C) 1994-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTextChooseSaveFile
#define _H_JTextChooseSaveFile

#include "jx-af/jcore/JPtrArray-JString.h"

class JTextChooseSaveFile
{
public:

	JTextChooseSaveFile();

	~JTextChooseSaveFile();

	bool ChooseFile(const JString& prompt,
					const JString& instructions,		// can be nullptr
					JString* fullName);
	bool ChooseFile(const JString& prompt,
					const JString& instructions,		// can be nullptr
					const JString& origName,			// can be nullptr
					JString* fullName);
	bool ChooseFiles(const JString& prompt,
					 const JString& instructions,	// can be nullptr
					 JPtrArray<JString>* fullNameList);

	bool ChooseRPath(const JString& prompt,
					 const JString& instructions,	// can be nullptr
					 const JString& origPath,		// can be nullptr
					 JString* newPath);
	bool ChooseRWPath(const JString& prompt,
					  const JString& instructions,	// can be nullptr
					  const JString& origPath,		// can be nullptr
					  JString* newPath);

	bool SaveFile(const JString& prompt,
				  const JString& instructions,		// can be nullptr
				  const JString& originalName,
				  JString* newFullName);

private:

	bool	GetPath(const JString& prompt,
					const JString& instructions,
					JString* newPath);
	bool	DoSystemCommand(const JString& str) const;
};

#endif
