/******************************************************************************
 JXFileDocument.h

	Interface for the JXFileDocument class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileDocument
#define _H_JXFileDocument

#include "JXDocument.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JError.h>
#include <jx-af/jcore/jTime.h>

class JXSaveFileDialog;

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

	~JXFileDocument() override;

	bool	NeedsSave() const override;
	bool	Save();
	void	DataModified();
	void	DataReverted(const bool fromUndo = false);

	bool	SaveInCurrentFile();
	bool	SaveInNewFile(const JString& fullName = JString::empty);
	bool	SaveCopyInNewFile(const JString& origName = JString::empty,
							  JString* fullName = nullptr);

	bool			ExistsOnDisk() const;
	const JString&	GetFilePath() const;
	const JString&	GetFileName() const;
	JString			GetFullName(bool* onDisk) const;
	const JString&	GetName() const override;

	bool	FileModifiedByOthers(bool* modTimeChanged = nullptr,
								 bool* permsChanged = nullptr) const;
	void	CheckIfModifiedByOthers();
	void	RevertIfChangedByOthers(const bool force = false);

	bool	WillMakeBackupFile() const;
	void	ShouldMakeBackupFile(const bool wantBackup);

	bool	WillMakeNewBackupEveryOpen() const;
	void	ShouldMakeNewBackupEveryOpen(const bool makeBackup);

	bool	WillAutosaveBeforeClosing() const;
	void	ShouldAutosaveBeforeClosing(const bool autosave);

	bool	WillAllocateTitleSpace() const;
	void	ShouldAllocateTitleSpace(const bool allocateSpace);

	static bool	WillAskOKToClose();
	static void	ShouldAskOKToClose(const bool ask);

	void			SafetySave(const JXDocumentManager::SafetySaveReason reason) override;
	bool			GetSafetySaveFileName(JString* fileName) const;
	static bool		CheckForSafetySaveFiles(const JString& fullName,
											JPtrArray<JString>* filesToOpen);

	const JString&		GetSaveBeforeClosePrompt() const;
	const JString&		GetSaveNewFilePrompt() const;
	const JString&		GetOKToRevertPrompt() const;

	static const JUtf8Byte*	SkipNeedsSavePrefix(const JUtf8Byte* s);

	// saving setup information

	void	ReadJXFDSetup(std::istream& input);
	void	WriteJXFDSetup(std::ostream& output) const;

protected:

	JXFileDocument(JXDirector* supervisor, const JString& fileName,
				   const bool onDisk, const bool wantBackupFile,
				   const JUtf8Byte* defaultFileNameSuffix);

	void	AdjustWindowTitle();
	void	FileChanged(const JString& fileName, const bool onDisk);

	bool			OKToClose() override;
	bool			OKToRevert() override;
	bool			CanRevert() override;
	virtual void	HandleFileModifiedByOthers(const bool modTimeChanged,
											   const bool permsChanged);
	virtual JError	WriteFile(const JString& fullName, const bool safetySave) const;
	virtual void	WriteTextFile(std::ostream& output, const bool safetySave) const;

	virtual JString	GetWindowTitle() const;

	virtual JXSaveFileDialog*	CreateSaveFileDialog(const JString& startName);

	static FileStatus	DefaultCanReadASCIIFile(std::istream& input,
												const JUtf8Byte* fileSignature,
												const JFileVersion currFileVersion,
												JFileVersion* actualFileVersion);

	void	SetSaveBeforeClosePrompt(const JString& prompt);
	void	SetSaveNewFilePrompt(const JString& prompt);
	void	SetOKToRevertPrompt(const JString& prompt);

private:

	JString	itsFilePath;
	JString	itsFileName;
	JString	itsFileSuffix;
	bool	itsWasOnDiskFlag;
	bool	itsSavedFlag;
	bool	itsWantBackupFileFlag;			// true if derived class wants backup file
	bool	itsMakeBackupFileFlag;			// true if should make backup before saving
	bool	itsWantNewBackupEveryOpenFlag;	// true if replace backup before first save
	bool	itsIsFirstSaveFlag;				// true if user hasn't saved yet
	bool	itsAutosaveBeforeCloseFlag;		// true if always save before closing
	time_t	itsFileModTime;
	bool	itsCheckModTimeFlag;
	mode_t	itsFilePerms;
	bool	itsCheckPermsFlag;
	bool	itsAllocateTitleSpaceFlag;

	bool		itsNeedSafetySaveFlag;		// true if not safety saved after latest DataModified()
	JString*	itsSafetySaveFileName;		// not nullptr if safety save file exists

	JString	itsSaveBeforeClosePrompt;
	JString	itsSaveNewFilePrompt;
	JString	itsOKToRevertPrompt;

	static bool	itsAskOKToCloseFlag;		// true if should ask "Save before closing?"

private:

	JString	GetFileNameForSave() const;
	void	RemoveSafetySaveFile();

public:

	// JBroadcaster messages

	static const JUtf8Byte* kNameChanged;

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

	static const JUtf8Byte* kWriteFailed;

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
		itsSavedFlag = false;
		AdjustWindowTitle();
		}

	itsNeedSafetySaveFlag = true;
}

/******************************************************************************
 Save

	Returns true if successful.

 ******************************************************************************/

inline bool
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

inline bool
JXFileDocument::WillMakeBackupFile()
	const
{
	return itsWantBackupFileFlag;
}

inline void
JXFileDocument::ShouldMakeBackupFile
	(
	const bool wantBackup
	)
{
	itsWantBackupFileFlag = wantBackup;
	itsMakeBackupFileFlag = itsWasOnDiskFlag && itsWantBackupFileFlag;
}

/******************************************************************************
 When to make backup file

	This controls whether a backup file is created only if none exists
	or whenever the file is first saved after being opened.

 ******************************************************************************/

inline bool
JXFileDocument::WillMakeNewBackupEveryOpen()
	const
{
	return itsWantNewBackupEveryOpenFlag;
}

inline void
JXFileDocument::ShouldMakeNewBackupEveryOpen
	(
	const bool makeBackup
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

inline bool
JXFileDocument::WillAutosaveBeforeClosing()
	const
{
	return itsAutosaveBeforeCloseFlag;
}

inline void
JXFileDocument::ShouldAutosaveBeforeClosing
	(
	const bool autosave
	)
{
	itsAutosaveBeforeCloseFlag = autosave;
}

/******************************************************************************
 Allocate space for "***" in window title

 ******************************************************************************/

inline bool
JXFileDocument::WillAllocateTitleSpace()
	const
{
	return itsAllocateTitleSpaceFlag;
}

inline void
JXFileDocument::ShouldAllocateTitleSpace
	(
	const bool allocateSpace
	)
{
	itsAllocateTitleSpaceFlag = allocateSpace;
	AdjustWindowTitle();
}

/******************************************************************************
 Ask "Save before closing?" (static)

 ******************************************************************************/

inline bool
JXFileDocument::WillAskOKToClose()
{
	return itsAskOKToCloseFlag;
}

inline void
JXFileDocument::ShouldAskOKToClose
	(
	const bool ask
	)
{
	itsAskOKToCloseFlag = ask;
}

/******************************************************************************
 Prompts

 ******************************************************************************/

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
