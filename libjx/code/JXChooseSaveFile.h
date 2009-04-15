/******************************************************************************
 JXChooseSaveFile.h

	Interface for the JXChooseSaveFile class.

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseSaveFile
#define _H_JXChooseSaveFile

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JChooseSaveFile.h>
#include <JPrefObject.h>

class JDirInfo;
class JXDirector;
class JXCSFDialogBase;
class JXChooseFileDialog;
class JXChoosePathDialog;
class JXSaveFileDialog;

class JXChooseSaveFile : public JChooseSaveFile, public JPrefObject,
						 virtual public JBroadcaster
{
public:

	JXChooseSaveFile(JPrefsManager* prefsMgr = NULL,
					 const JPrefID& id = JFAID::kInvalidID);

	virtual ~JXChooseSaveFile();

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

	JBoolean ChooseFile(const JCharacter* prompt,
						const JCharacter* instructions,		// can be NULL
						const JCharacter* wildcardFilter,
						const JCharacter* origName,			// can be NULL
						JString* fullName);
	JBoolean ChooseFiles(const JCharacter* prompt,
						 const JCharacter* instructions,	// can be NULL
						 const JCharacter* wildcardFilter,
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

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

	static JBoolean	IsCharacterInWord(const JString& text, const JIndex charIndex);

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean allowSelectMultiple,
						   const JCharacter* origName, const JCharacter* message);

	virtual void	SetChooseFileFilters(JDirInfo* dirInfo);

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter,
						   const JBoolean selectOnlyWritable, const JCharacter* message);

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JCharacter* fileFilter, const JCharacter* origName,
						 const JCharacter* prompt, const JCharacter* message);

	JDirInfo*		GetDirInfo();

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JDirInfo*	itsDirInfo;			// NULL if not global JXCSF stored in jXGlobals
	JString*	itsUserFilter;
	JString*	itsDialogState;		// empty if not global JXCSF stored in jXGlobals

	JXChooseFileDialog*	itsChooseFileDialog;
	JXChoosePathDialog*	itsChoosePathDialog;
	JXSaveFileDialog*	itsSaveFileDialog;
	JXCSFDialogBase*	itsCurrentDialog;

	JBoolean			itsResponse;
	JString*			itsResultStr;	// not owned; non-NULL if any other dialog is open
	JPtrArray<JString>*	itsResultList;	// not owned; non-NULL if choose multiple file dialog is open

private:

	JBoolean	ChooseFile(const JCharacter* prompt,
						   const JCharacter* instructions,		// can be NULL
						   const JCharacter* origName,			// can be NULL
						   const JBoolean allowSelectMultiple);

	JBoolean	ChoosePath(const JBoolean selectOnlyWritable,
						   const JCharacter* instructions,
						   const JCharacter* origPath,
						   JString* newPath);

	void	WaitForResponse(JXCSFDialogBase* dlog);

	void	RestoreState(JXCSFDialogBase* dlog,
						 const JBoolean ignoreScroll) const;
	void	SaveState(JXCSFDialogBase* dlog);

	JString*	GetDialogState() const;

	// not allowed

	JXChooseSaveFile(const JXChooseSaveFile& source);
	const JXChooseSaveFile& operator=(const JXChooseSaveFile& source);
};

#endif
