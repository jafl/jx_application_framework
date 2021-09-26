/******************************************************************************
 JTextChooseSaveFile.h

	Interface for the JTextChooseSaveFile class.

	Copyright (C) 1994-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTextChooseSaveFile
#define _H_JTextChooseSaveFile

#include "jx-af/jcore/JChooseSaveFile.h"

class JTextChooseSaveFile : public JChooseSaveFile
{
public:

	JTextChooseSaveFile();

	virtual ~JTextChooseSaveFile();

	virtual bool ChooseFile(const JString& prompt,
								const JString& instructions,		// can be nullptr
								JString* fullName);
	virtual bool ChooseFile(const JString& prompt,
								const JString& instructions,		// can be nullptr
								const JString& origName,			// can be nullptr
								JString* fullName);
	virtual bool ChooseFiles(const JString& prompt,
								 const JString& instructions,	// can be nullptr
								 JPtrArray<JString>* fullNameList);

	virtual bool ChooseRPath(const JString& prompt,
								 const JString& instructions,	// can be nullptr
								 const JString& origPath,		// can be nullptr
								 JString* newPath);
	virtual bool ChooseRWPath(const JString& prompt,
								  const JString& instructions,	// can be nullptr
								  const JString& origPath,		// can be nullptr
								  JString* newPath);

	virtual bool SaveFile(const JString& prompt,
							  const JString& instructions,		// can be nullptr
							  const JString& originalName,
							  JString* newFullName);

private:

	bool	GetPath(const JString& prompt,
						const JString& instructions,
						JString* newPath);
	bool	DoSystemCommand(const JString& str) const;

	// not allowed

	JTextChooseSaveFile(const JTextChooseSaveFile& source);
	const JTextChooseSaveFile& operator=(const JTextChooseSaveFile& source);
};

#endif
