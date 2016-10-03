/******************************************************************************
 JXFileDocument.h

	Interface for the JXFileDocument class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFileDocument
#define _H_JXFileDocument

#include <JXDocument.h>
#include <JString.h>
#include <JError.h>
#include <jTime.h>

class JChooseSaveFile;

class JXFileDocument : public JXDocument
{
public:

	enum FileStatus
	{
		kFileReadable,
		kNeedNewerVersion,
		kNotMyFile,
		kFileNotReadable
	};

public:

	virtual ~JXFileDocument();

	virtual JBoolean	NeedsSave() const;
	JBoolean			Save();
	void				DataModified();
	void				DataReverted(const JBoolean fromUndo = kJFalse);

	JBoolean	SaveInCurrentFile();
	JBoolean	SaveInNewFile(const JCharacter* fullName = NULL);
	JBoolean	SaveCopyInNewFile(const JCharacter* origName = NULL,
								  JString* fullName = NULL);

	JBoolean				ExistsOnDisk() const;
	const JString&			GetFilePath() const;
	const JString&			GetFileName() const;
	JString					GetFullName(JBoolean* onDisk) const;
	virtual const JString&	GetName() const;

	JBoolean	FileModifiedByOthers(JBoolean* modTimeChanged = NULL,
									 JBoolean* permsChanged = NULL) const;
	void		CheckIfModifiedByOthers();
	void		RevertIfChangedByOthers(const JBoolean force = kJFalse);

	JBoolean	WillMakeBackupFile() const;
	void		ShouldMakeBackupFile(const JBoolean wantBackup);

	JBoolean	WillMakeNewBackupEveryOpen() const;
	void		ShouldMakeNewBackupEveryOpen(const JBoolean makeBackup);

	JBoolean	WillAutosaveBeforeClosing() const;
	void		ShouldAutosaveBeforeClosing(const JBoolean autosave);

	JBoolean	WillAllocateTitleSpace() const;
	void		ShouldAllocateTitleSpace(const JBoolean allocateSpace);

	static JBoolean	WillAskOKToClose();
	static void		ShouldAskOKToClose(const JBoolean ask);

	virtual void	SafetySave(const JXDocumentManager::SafetySaveReason reason);
	JBoolean		GetSafetySaveFileName(JString* fileName) const;
	static JBoolean	CheckForSafetySaveFiles(const JCharacter* fullName,
											JPtrArray<JString>* filesToOpen);

	JChooseSaveFile*	GetChooseSaveFile() const;
	const JString&		GetSaveBeforeClosePrompt() const;
	const JString&		GetSaveNewFilePrompt() const;
	const JString&		GetOKToRevertPrompt() const;

	static const JCharacter*	SkipNeedsSavePrefix(const JCharacter* s);

	// saving setup information

	void	ReadJXFDSetup(istream& input);
	void	WriteJXFDSetup(ostream& output) const;

protected:

	JXFileDocument(JXDirector* supervisor, const JCharacter* fileName,
				   const JBoolean onDisk, const JBoolean wantBackupFile,
				   const JCharacter* defaultFileNameSuffix);

	void		AdjustWindowTitle();
	void		FileChanged(const JCharacter* fileName, const JBoolean onDisk);

	virtual JBoolean	OKToClose();
	virtual JBoolean	OKToRevert();
	virtual JBoolean	CanRevert();
	virtual void		HandleFileModifiedByOthers(const JBoolean modTimeChanged,
												   const JBoolean permsChanged);
	virtual JError		WriteFile(const JCharacter* fullName, const JBoolean safetySave) const;
	virtual void		WriteTextFile(ostream& output, const JBoolean safetySave) const;

	virtual JString		GetWindowTitle() const;

	static FileStatus	DefaultCanReadASCIIFile(istream& input,
												const JCharacter* fileSignature,
												const JFileVersion currFileVersion,
												JFileVersion* actualFileVersion);

	void	SetChooseSaveFile(JChooseSaveFile* csf);
	void	SetSaveBeforeClosePrompt(const JCharacter* prompt);
	void	SetSaveNewFilePrompt(const JCharacter* prompt);
	void	SetOKToRevertPrompt(const JCharacter* prompt);

private:

	JString		itsFilePath;
	JString		itsFileName;
	JString		itsFileSuffix;
	JBoolean	itsWasOnDiskFlag;
	JBoolean	itsSavedFlag;
	JBoolean	itsWantBackupFileFlag;			// kJTrue if derived class wants backup file
	JBoolean	itsMakeBackupFileFlag;			// kJTrue if should make backup before saving
	JBoolean	itsWantNewBackupEveryOpenFlag;	// kJTrue if replace backup before first save
	JBoolean	itsIsFirstSaveFlag;				// kJTrue if user hasn't saved yet
	JBoolean	itsAutosaveBeforeCloseFlag;		// kJTrue if always save before closing
	time_t		itsFileModTime;
	JBoolean	itsCheckModTimeFlag;
	mode_t		itsFilePerms;
	JBoolean	itsCheckPermsFlag;
	JBoolean	itsAllocateTitleSpaceFlag;

	JBoolean	itsNeedSafetySaveFlag;	// kJTrue if not safety saved after latest DataModified()
	JString*	itsSafetySaveFileName;	// not NULL if safety save file exists

	JChooseSaveFile*	itsCSF;			// we don't own this
	JString				itsSaveBeforeClosePrompt;
	JString				itsSaveNewFilePrompt;
	JString				itsOKToRevertPrompt;

	static JBoolean	itsAskOKToCloseFlag;		// kJTrue if should ask "Save before closing?"

private:

	JString		GetFileNameForSave() const;
	JBoolean	SaveInCurrentFile1();
	void		RemoveSafetySaveFile();

	// not allowed

	JXFileDocument(const JXFileDocument& source);
	const JXFileDocument& operator=(const JXFileDocument& source);

public:

	// JBroadcaster messages

	static const JCharacter* kNameChanged;

	class NameChanged : public JBroadcaster::Message
		{
		public:

			NameChanged(const JString& fullName)
				:
				JBroadcaster::Message(kNameChanged),
				itsFullName(fullName)
				{ };

			const JString&
			GetFullName() const
			{
				return itsFullName;
			}

		private:

			const JString& itsFullName;
		};

public:

	// JError messages

	static const JCharacter* kWriteFailed;

	class WriteFailed : public JError
	{
	public:

		WriteFailed()
			:
			JError(kWriteFailed)
			{ };
	};
};


/*****************************************************************************
 File path and name

 ******************************************************************************/

inline const JString&
JXFileDocument::GetFilePath()
	const
{
	return itsFilePath;
}

inline const JString&
JXFileDocument::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 DataModified

 ******************************************************************************/

inline void
JXFileDocument::DataModified()
{
	if (itsSavedFlag)
		{
		itsSavedFlag = kJFalse;
		AdjustWindowTitle();
		}

	itsNeedSafetySaveFlag = kJTrue;
}

/******************************************************************************
 SetChooseSaveFile (protected)

 ******************************************************************************/

inline void
JXFileDocument::SetChooseSaveFile
	(
	JChooseSaveFile* csf
	)
{
	itsCSF = csf;
}

/******************************************************************************
 Save

	Returns kJTrue if successful.

 ******************************************************************************/

inline JBoolean
JXFileDocument::Save()
{
	if (!itsSavedFlag)
		{
		SaveInCurrentFile();
		}

	return itsSavedFlag;
}

/******************************************************************************
 Make backup file

 ******************************************************************************/

inline JBoolean
JXFileDocument::WillMakeBackupFile()
	const
{
	return itsWantBackupFileFlag;
}

inline void
JXFileDocument::ShouldMakeBackupFile
	(
	const JBoolean wantBackup
	)
{
	itsWantBackupFileFlag = wantBackup;
	itsMakeBackupFileFlag = JConvertToBoolean( itsWasOnDiskFlag && itsWantBackupFileFlag );
}

/******************************************************************************
 When to make backup file

	This controls whether a backup file is created only if none exists
	or whenever the file is first saved after being opened.

 ******************************************************************************/

inline JBoolean
JXFileDocument::WillMakeNewBackupEveryOpen()
	const
{
	return itsWantNewBackupEveryOpenFlag;
}

inline void
JXFileDocument::ShouldMakeNewBackupEveryOpen
	(
	const JBoolean makeBackup
	)
{
	itsWantNewBackupEveryOpenFlag = makeBackup;
}

/******************************************************************************
 Autosave before closing

	This controls whether the file is always saved before closing.  This is
	usually a good idea because data files should store things like window
	positions, scrollbar setup, etc., which shouldn't bother the user with a
	"Save before closing?" message, but should still be updated when the
	document is closed.

 ******************************************************************************/

inline JBoolean
JXFileDocument::WillAutosaveBeforeClosing()
	const
{
	return itsAutosaveBeforeCloseFlag;
}

inline void
JXFileDocument::ShouldAutosaveBeforeClosing
	(
	const JBoolean autosave
	)
{
	itsAutosaveBeforeCloseFlag = autosave;
}

/******************************************************************************
 Allocate space for "***" in window title

 ******************************************************************************/

inline JBoolean
JXFileDocument::WillAllocateTitleSpace()
	const
{
	return itsAllocateTitleSpaceFlag;
}

inline void
JXFileDocument::ShouldAllocateTitleSpace
	(
	const JBoolean allocateSpace
	)
{
	itsAllocateTitleSpaceFlag = allocateSpace;
	AdjustWindowTitle();
}

/******************************************************************************
 Ask "Save before closing?" (static)

 ******************************************************************************/

inline JBoolean
JXFileDocument::WillAskOKToClose()
{
	return itsAskOKToCloseFlag;
}

inline void
JXFileDocument::ShouldAskOKToClose
	(
	const JBoolean ask
	)
{
	itsAskOKToCloseFlag = ask;
}

/******************************************************************************
 Choose/Save file

 ******************************************************************************/

inline JChooseSaveFile*
JXFileDocument::GetChooseSaveFile()
	const
{
	return itsCSF;
}

inline const JString&
JXFileDocument::GetSaveBeforeClosePrompt()
	const
{
	return itsSaveBeforeClosePrompt;
}

inline const JString&
JXFileDocument::GetSaveNewFilePrompt()
	const
{
	return itsSaveNewFilePrompt;
}

inline const JString&
JXFileDocument::GetOKToRevertPrompt()
	const
{
	return itsOKToRevertPrompt;
}

#endif
