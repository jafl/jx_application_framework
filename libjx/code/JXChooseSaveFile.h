/******************************************************************************
 JXChooseSaveFile.h

	Interface for the JXChooseSaveFile class.

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseSaveFile
#define _H_JXChooseSaveFile

#include <jx-af/jcore/JChooseSaveFile.h>
#include <jx-af/jcore/JPrefObject.h>

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

	JXChooseSaveFile(JPrefsManager* prefsMgr = nullptr,
					 const JPrefID& id = JFAID::kInvalidID);

	~JXChooseSaveFile();

	bool ChooseFile(const JString& prompt,
					const JString& instructions,
					JString* fullName) override;
	bool ChooseFile(const JString& prompt,
					const JString& instructions,
					const JString& origName,
					JString* fullName) override;
	bool ChooseFiles(const JString& prompt,
					 const JString& instructions,
					 JPtrArray<JString>* fullNameList) override;

	bool ChooseFile(const JString& prompt,
					const JString& instructions,
					const JString& wildcardFilter,
					const JString& origName,
					JString* fullName);
	bool ChooseFiles(const JString& prompt,
					 const JString& instructions,
					 const JString& wildcardFilter,
					 JPtrArray<JString>* fullNameList);

	bool ChooseRPath(const JString& prompt,
					 const JString& instructions,
					 const JString& origPath,
					 JString* newPath) override;
	bool ChooseRWPath(const JString& prompt,
					  const JString& instructions,
					  const JString& origPath,
					  JString* newPath) override;

	bool SaveFile(const JString& prompt,
				  const JString& instructions,
				  const JString& originalName,
				  JString* newFullName) override;

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	static bool	IsCharacterInWord(const JUtf8Character& c);

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool allowSelectMultiple,
						   const JString& origName, const JString& message);

	virtual void	SetChooseFileFilters(JDirInfo* dirInfo);

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool selectOnlyWritable, const JString& message);

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter, const JString& origName,
						 const JString& prompt, const JString& message);

	JDirInfo*	GetDirInfo();

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JDirInfo*	itsDirInfo;			// nullptr if not global JXCSF stored in jXGlobals
	JString		itsUserFilter;
	JString		itsDialogState;		// empty if not global JXCSF stored in jXGlobals

	JXChooseFileDialog*	itsChooseFileDialog;
	JXChoosePathDialog*	itsChoosePathDialog;
	JXSaveFileDialog*	itsSaveFileDialog;
	JXCSFDialogBase*	itsCurrentDialog;

	bool				itsResponse;
	JString*			itsResultStr;	// not owned; non-nullptr if any other dialog is open
	JPtrArray<JString>*	itsResultList;	// not owned; non-nullptr if choose multiple file dialog is open

private:

	bool	ChooseFile(const JString& prompt,
					   const JString& instructions,
					   const JString& origName,
					   const bool allowSelectMultiple);

	bool	ChoosePath(const bool selectOnlyWritable,
					   const JString& instructions,
					   const JString& origPath,
					   JString* newPath);

	void	WaitForResponse(JXCSFDialogBase* dlog);

	void	RestoreState(JXCSFDialogBase* dlog,
						 const bool ignoreScroll);
	void	SaveState(JXCSFDialogBase* dlog);

	const JString&	GetDialogState() const;
	JString*		GetDialogState();
};

#endif
