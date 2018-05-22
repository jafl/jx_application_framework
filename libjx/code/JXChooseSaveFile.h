/******************************************************************************
 JXChooseSaveFile.h

	Interface for the JXChooseSaveFile class.

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseSaveFile
#define _H_JXChooseSaveFile

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

	virtual JBoolean ChooseFile(const JString& prompt,
								const JString& instructions,
								JString* fullName) override;
	virtual JBoolean ChooseFile(const JString& prompt,
								const JString& instructions,
								const JString& origName,
								JString* fullName) override;
	virtual JBoolean ChooseFiles(const JString& prompt,
								 const JString& instructions,
								 JPtrArray<JString>* fullNameList) override;

	JBoolean ChooseFile(const JString& prompt,
						const JString& instructions,
						const JString& wildcardFilter,
						const JString& origName,
						JString* fullName);
	JBoolean ChooseFiles(const JString& prompt,
						 const JString& instructions,
						 const JString& wildcardFilter,
						 JPtrArray<JString>* fullNameList);

	virtual JBoolean ChooseRPath(const JString& prompt,
								 const JString& instructions,
								 const JString& origPath,
								 JString* newPath) override;
	virtual JBoolean ChooseRWPath(const JString& prompt,
								  const JString& instructions,
								  const JString& origPath,
								  JString* newPath) override;

	virtual JBoolean SaveFile(const JString& prompt,
							  const JString& instructions,
							  const JString& originalName,
							  JString* newFullName) override;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	static JBoolean	IsCharacterInWord(const JUtf8Character& c);

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const JBoolean allowSelectMultiple,
						   const JString& origName, const JString& message);

	virtual void	SetChooseFileFilters(JDirInfo* dirInfo);

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const JBoolean selectOnlyWritable, const JString& message);

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter, const JString& origName,
						 const JString& prompt, const JString& message);

	JDirInfo*		GetDirInfo();

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JDirInfo*	itsDirInfo;			// NULL if not global JXCSF stored in jXGlobals
	JString		itsUserFilter;
	JString		itsDialogState;		// empty if not global JXCSF stored in jXGlobals

	JXChooseFileDialog*	itsChooseFileDialog;
	JXChoosePathDialog*	itsChoosePathDialog;
	JXSaveFileDialog*	itsSaveFileDialog;
	JXCSFDialogBase*	itsCurrentDialog;

	JBoolean			itsResponse;
	JString*			itsResultStr;	// not owned; non-NULL if any other dialog is open
	JPtrArray<JString>*	itsResultList;	// not owned; non-NULL if choose multiple file dialog is open

private:

	JBoolean	ChooseFile(const JString& prompt,
						   const JString& instructions,
						   const JString& origName,
						   const JBoolean allowSelectMultiple);

	JBoolean	ChoosePath(const JBoolean selectOnlyWritable,
						   const JString& instructions,
						   const JString& origPath,
						   JString* newPath);

	void	WaitForResponse(JXCSFDialogBase* dlog);

	void	RestoreState(JXCSFDialogBase* dlog,
						 const JBoolean ignoreScroll);
	void	SaveState(JXCSFDialogBase* dlog);

	const JString&	GetDialogState() const;
	JString*		GetDialogState();

	// not allowed

	JXChooseSaveFile(const JXChooseSaveFile& source);
	const JXChooseSaveFile& operator=(const JXChooseSaveFile& source);
};

#endif
