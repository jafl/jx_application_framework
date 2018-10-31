/******************************************************************************
 JXFileDocument.cpp

	Document class that represents data stored in any type of file.  This can
	be used for a multi-file document if there is a unique file that the
	user selects to open the document.  This particular file can then be
	the file stored by JXFileDocument.

	Derived classes should not implement OKToClose().  Instead, they must
	implement either WriteFile() or WriteTextFile().  For documents stored
	in text files (the easiest, safest, and most portable method), simply
	override WriteTextFile().  For other types of files, override WriteFile().

	The safetySave flag passed to WriteFile() and WriteTextFile() should
	obviously not affect what is written, but it can affect other actions.
	As an example, see TestTextEditDocument::WriteFile() in testjx.

	CheckForSafetySaveFiles() is provided to look for safety saved versions
	of files that the user opens.  We cannot provide the equivalent feature
	for untitled documents because, while we could easily check for #untitled#_
	files in the user's home directory, we do not know which application created
	them.

	BASE CLASS = JXDocument

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JXFileDocument.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <JString.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

JBoolean JXFileDocument::itsAskOKToCloseFlag = kJTrue;

static const JUtf8Byte* kBackupFileSuffix = "~";

static const JUtf8Byte* kSafetySavePrefix = "#";
static const JUtf8Byte* kSafetySaveSuffix = "#";

static const JUtf8Byte* kAssertSavePrefix = "##";
static const JUtf8Byte* kAssertSaveSuffix = "##";

// setup information

const JFileVersion kCurrentSetupVersion = 1;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

	// version 1 stores itsWantNewBackupEveryOpenFlag

// JBroadcaster message types

const JUtf8Byte* JXFileDocument::kNameChanged = "NameChanged::JXFileDocument";

// JError messages

const JUtf8Byte* JXFileDocument::kWriteFailed = "WriteFailed::JXFileDocument";

/******************************************************************************
 Constructor

	Derived class should call AdjustWindowTitle() after creating a window.

	defaultFileNameSuffix can be the empty string.  If it isn't,
	SaveInNewFile() appends it to the file name before opening the
	"Save file" dialog.

 ******************************************************************************/

JXFileDocument::JXFileDocument
	(
	JXDirector*			supervisor,
	const JString&		fileName,
	const JBoolean		onDisk,
	const JBoolean		wantBackupFile,
	const JUtf8Byte*	defaultFileNameSuffix
	)
	:
	JXDocument(supervisor),
	itsFileSuffix(defaultFileNameSuffix),
	itsSaveBeforeClosePrompt(JGetString("SaveBeforeClosePrompt::JXFileDocument")),
	itsSaveNewFilePrompt(JGetString("SaveNewFilePrompt::JXFileDocument")),
	itsOKToRevertPrompt(JGetString("OKToRevertPrompt::JXFileDocument"))
{
	itsAllocateTitleSpaceFlag     = kJFalse;
	itsWantBackupFileFlag         = wantBackupFile;
	itsWantNewBackupEveryOpenFlag = kJTrue;
	itsAutosaveBeforeCloseFlag    = kJFalse;
	itsCSF                        = JGetChooseSaveFile();
	itsNeedSafetySaveFlag         = kJFalse;
	itsSafetySaveFileName         = nullptr;

	FileChanged(fileName, onDisk);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileDocument::~JXFileDocument()
{
	RemoveSafetySaveFile();
}

/******************************************************************************
 ReadJXFDSetup

 ******************************************************************************/

void
JXFileDocument::ReadJXFDSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
		{
		JBoolean wantBackup, allocTitleSpace;
		input >> wantBackup >> allocTitleSpace;
		ShouldMakeBackupFile(wantBackup);
		ShouldAllocateTitleSpace(allocTitleSpace);

		if (vers >= 1)
			{
			JBoolean newBackupEveryOpen;
			input >> newBackupEveryOpen;
			ShouldMakeNewBackupEveryOpen(newBackupEveryOpen);
			}
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteJXFDSetup

 ******************************************************************************/

void
JXFileDocument::WriteJXFDSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsWantBackupFileFlag;
	output << ' ' << itsAllocateTitleSpaceFlag;
	output << ' ' << itsWantNewBackupEveryOpenFlag;
	output << kSetupDataEndDelimiter;
}

/******************************************************************************
 ExistsOnDisk

 ******************************************************************************/

JBoolean
JXFileDocument::ExistsOnDisk()
	const
{
	const JString fullName = itsFilePath + itsFileName;
	return JConvertToBoolean( itsWasOnDiskFlag && JFileExists(fullName) );
}

/*****************************************************************************
 GetFullName

 ******************************************************************************/

JString
JXFileDocument::GetFullName
	(
	JBoolean* onDisk
	)
	const
{
	const JString fullName = itsFilePath + itsFileName;
	*onDisk = JConvertToBoolean( itsWasOnDiskFlag && JFileExists(fullName) );
	return fullName;
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
JXFileDocument::GetName()
	const
{
	return itsFileName;
}

/******************************************************************************
 NeedsSave (virtual)

	Implementation of JXDocument::NeedsSave().

 ******************************************************************************/

JBoolean
JXFileDocument::NeedsSave()
	const
{
	return !itsSavedFlag;
}

/******************************************************************************
 FileChanged (protected)

	Call this to notify us that the document is now displaying a different file.
	(e.g. after New or Open on the File menu)

 ******************************************************************************/

void
JXFileDocument::FileChanged
	(
	const JString&	fileName,
	const JBoolean	onDisk
	)
{
	itsSavedFlag          = kJTrue;
	itsWasOnDiskFlag      = onDisk;
	itsMakeBackupFileFlag = JConvertToBoolean( itsWasOnDiskFlag && itsWantBackupFileFlag );
	itsIsFirstSaveFlag    = kJTrue;

	if (onDisk)
		{
		JString fullName;
		const JBoolean ok = JGetTrueName(fileName, &fullName);
		assert( ok );
		JSplitPathAndName(fullName, &itsFilePath, &itsFileName);

		JError err = JGetModificationTime(fileName, &itsFileModTime);
		assert_ok( err );
		itsCheckModTimeFlag = kJTrue;

		err = JGetPermissions(fileName, &itsFilePerms);
		assert_ok( err );
		itsCheckPermsFlag = kJTrue;
		}
	else
		{
		itsFilePath.Clear();
		itsFileName         = fileName;
		itsFileModTime      = 0;
		itsCheckModTimeFlag = kJFalse;
		itsFilePerms        = 0;
		itsCheckPermsFlag   = kJFalse;
		}

	RemoveSafetySaveFile();
	AdjustWindowTitle();

	const JString fullName = itsFilePath + itsFileName;
	Broadcast(NameChanged(fullName));
}

/******************************************************************************
 OKToClose (virtual protected)

 ******************************************************************************/

JBoolean
JXFileDocument::OKToClose()
{
	if (itsSavedFlag)
		{
		if (itsAutosaveBeforeCloseFlag && itsWasOnDiskFlag)
			{
			SaveInCurrentFile();
			}
		return itsSavedFlag;
		}

	Activate();		// show ourselves so user knows what we are asking about

	JString msg = itsSaveBeforeClosePrompt;
	const JUtf8Byte* map[] =
		{
		"name", itsFileName.GetBytes()
		};
	(JGetStringManager())->Replace(&msg, map, sizeof(map));

	const JUserNotification::CloseAction action =
		itsAskOKToCloseFlag ? (JGetUserNotification())->OKToClose(msg) :
							  JUserNotification::kSaveData;
	if (action == JUserNotification::kSaveData)
		{
		SaveInCurrentFile();
		return itsSavedFlag;
		}
	else if (action == JUserNotification::kDiscardData)
		{
		return kJTrue;
		}
	else
		{
		assert( action == JUserNotification::kDontClose );
		return kJFalse;
		}
}

/******************************************************************************
 OKToRevert (virtual protected)

	If the data hasn't been saved or the file has been changed behind
	our back, asks the user if it is ok to revert to the data stored in
	the file.

 ******************************************************************************/

JBoolean
JXFileDocument::OKToRevert()
{
	Activate();		// show ourselves so user knows what we are asking about

	if (FileModifiedByOthers())
		{
		const JUtf8Byte* map[] =
			{
			"name", itsFileName.GetBytes()
			};
		const JString msg = JGetString(
			itsSavedFlag? "OKToRevertSavedPrompt::JXFileDocument" : "OKToRevertUnsavedPrompt::JXFileDocument",
			map, sizeof(map));

		return (JGetUserNotification())->AskUserNo(msg);
		}

	else if (!itsSavedFlag && ExistsOnDisk())
		{
		JString msg = itsOKToRevertPrompt;
		const JUtf8Byte* map[] =
			{
			"name", itsFileName.GetBytes()
			};
		(JGetStringManager())->Replace(&msg, map, sizeof(map));
		return (JGetUserNotification())->AskUserYes(msg);
		}

	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CanRevert (virtual protected)

	Returns kJTrue if the data has not been saved or the file has
	been modified behind our back.

 ******************************************************************************/

JBoolean
JXFileDocument::CanRevert()
{
	if (!ExistsOnDisk())
		{
		return kJFalse;
		}
	else if (!itsSavedFlag)
		{
		return kJTrue;
		}

	return FileModifiedByOthers();
}

/******************************************************************************
 CheckIfModifiedByOthers

	Checks if the file exists and has been modified behind our back.

 ******************************************************************************/

void
JXFileDocument::CheckIfModifiedByOthers()
{
	JBoolean modTimeChanged, permsChanged;
	if (FileModifiedByOthers(&modTimeChanged, &permsChanged) || permsChanged)
		{
		HandleFileModifiedByOthers(modTimeChanged, permsChanged);
		}
}

/******************************************************************************
 HandleFileModifiedByOthers (virtual protected)

	The default is to do nothing.

 ******************************************************************************/

void
JXFileDocument::HandleFileModifiedByOthers
	(
	const JBoolean modTimeChanged,
	const JBoolean permsChanged
	)
{
}

/******************************************************************************
 FileModifiedByOthers

	Returns kJTrue if the file exists and has been modified behind our back.

 ******************************************************************************/

JBoolean
JXFileDocument::FileModifiedByOthers
	(
	JBoolean* modTimeChanged,
	JBoolean* permsChanged
	)
	const
{
	const JString fullName = itsFilePath + itsFileName;
	time_t modTime;
	mode_t perms;

	const JBoolean m = JI2B(
		itsCheckModTimeFlag &&
		JGetModificationTime(fullName, &modTime) == kJNoError &&
		modTime != itsFileModTime);
	if (modTimeChanged != nullptr)
		{
		*modTimeChanged = m;
		}

	const JBoolean p = JI2B(
		itsCheckPermsFlag &&
		JGetPermissions(fullName, &perms) == kJNoError &&
		perms != itsFilePerms);
	if (permsChanged != nullptr)
		{
		*permsChanged = p;
		}

	return m;
}

/******************************************************************************
 RevertIfChangedByOthers

 ******************************************************************************/

void
JXFileDocument::RevertIfChangedByOthers
	(
	const JBoolean force
	)
{
	if (!NeedsSave() && CanRevert())
		{
		if (force)
			{
			DiscardChanges();
			}
		else
			{
			RevertToSaved();
			}
		}
}

/******************************************************************************
 DataReverted

	Call this after a "Revert to saved" operation

 ******************************************************************************/

void
JXFileDocument::DataReverted
	(
	const JBoolean fromUndo
	)
{
	if (itsWasOnDiskFlag && !fromUndo)
		{
		const JString fullName = itsFilePath + itsFileName;
		time_t modTime;
		if (JGetModificationTime(fullName, &modTime) == kJNoError)
			{
			itsFileModTime      = modTime;
			itsCheckModTimeFlag = kJTrue;
			}
		mode_t perms;
		if (JGetPermissions(fullName, &perms) == kJNoError)
			{
			itsFilePerms      = perms;
			itsCheckPermsFlag = kJTrue;
			}
		}

	itsSavedFlag = kJTrue;
	RemoveSafetySaveFile();
	AdjustWindowTitle();
}

/******************************************************************************
 SaveCopyInNewFile

	Save a copy of the data in a new file.  Our file is not changed.
	Returns kJTrue if a file is successfully written.

	If origName is not empty, it is passed to JChooseSaveFile::SaveFile().

	If fullName != nullptr and the save is successful, *fullName is the name
	of the file that was created.

 ******************************************************************************/

JBoolean
JXFileDocument::SaveCopyInNewFile
	(
	const JString&	origUserName,
	JString*		fullName
	)
{
	JString origName;
	if (!origUserName.IsEmpty())
		{
		origName = origUserName;
		}
	else
		{
		origName = GetFileNameForSave();
		}

	JString newName;
	if (itsCSF->SaveFile(itsSaveNewFilePrompt, JString::empty, origName, &newName))
		{
		// We set safetySave because it's as if it were a temp file.

		const JError err = WriteFile(newName, kJTrue);
		if (err.OK())
			{
			if (fullName != nullptr)
				{
				*fullName = newName;
				}
			return kJTrue;
			}
		else
			{
			err.ReportIfError();
			}
		}

	if (fullName != nullptr)
		{
		fullName->Clear();
		}
	return kJFalse;
}

/******************************************************************************
 SaveInNewFile

	Save the data in a new file and update ourselves to refer to this new file.

 ******************************************************************************/

JBoolean
JXFileDocument::SaveInNewFile
	(
	const JString& newFullName
	)
{
	JString fullName;
	if (!newFullName.IsEmpty())
		{
		fullName = newFullName;
		}

	const JString origName = GetFileNameForSave();
	if (!fullName.IsEmpty() ||
		itsCSF->SaveFile(itsSaveNewFilePrompt, JString::empty, origName, &fullName))
		{
		const JString savePath    = itsFilePath;
		const JString saveName    = itsFileName;
		const JBoolean saveFlag[] =
			{
			itsWasOnDiskFlag,
			itsMakeBackupFileFlag,
			itsCheckModTimeFlag,
			itsCheckPermsFlag,
			itsSavedFlag,
			};

		JSplitPathAndName(fullName, &itsFilePath, &itsFileName);
		itsWasOnDiskFlag      = kJTrue;
		itsMakeBackupFileFlag = kJFalse;
		itsCheckModTimeFlag   = kJFalse;
		itsCheckPermsFlag     = kJFalse;
		itsSavedFlag          = kJFalse;
		SaveInCurrentFile();
		if (itsSavedFlag)
			{
			itsMakeBackupFileFlag = itsWantBackupFileFlag;	// make backup when save next time
			Broadcast(NameChanged(fullName));
			return kJTrue;
			}
		else
			{
			itsFilePath           = savePath;
			itsFileName           = saveName;
			itsWasOnDiskFlag      = saveFlag[0];
			itsMakeBackupFileFlag = saveFlag[1];
			itsCheckModTimeFlag   = saveFlag[2];
			itsCheckPermsFlag     = saveFlag[3];
			itsSavedFlag          = saveFlag[4];
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetFileNameForSave (private)

 ******************************************************************************/

JString
JXFileDocument::GetFileNameForSave()
	const
{
	JString fileName = itsFileName;
	if (!itsWasOnDiskFlag && !fileName.EndsWith(itsFileSuffix))
		{
		fileName += itsFileSuffix;
		}
	if (itsWasOnDiskFlag)
		{
		fileName = JCombinePathAndName(itsFilePath, fileName);
		}
	return fileName;
}

/******************************************************************************
 SaveInCurrentFile

 ******************************************************************************/

JBoolean
JXFileDocument::SaveInCurrentFile()
{
	if (!itsWasOnDiskFlag)
		{
		return SaveInNewFile();
		}

	const JString fullName     = itsFilePath + itsFileName;
	const JBoolean exists      = JFileExists(fullName);
	const JBoolean dirWritable = JDirectoryWritable(itsFilePath);
	if (exists && !JFileWritable(fullName))
		{
		JBoolean triedWrite = kJFalse;
		if ((JGetUserNotification())->AskUserYes(
				JGetString("TryChangeWriteProtect::JXFileDocument")))
			{
			mode_t perms;
			if (JGetPermissions(fullName, &perms)          == kJNoError &&
				JSetPermissions(fullName, perms | S_IWUSR) == kJNoError)
				{
				const JBoolean ok1 = JFileWritable(fullName);
				const JBoolean ok2 = JI2B(ok1 && SaveInCurrentFile());
				const JError err   = JSetPermissions(fullName, perms);
				if (!err.OK())
					{
					(JGetStringManager())->ReportError("NoRestoreWriteProtectError::JXFileDocument", err);
					}
				if (ok2)
					{
					return kJTrue;
					}
				else if (ok1 && !ok2)
					{
					return kJFalse;
					}
				}

			triedWrite = kJTrue;
			}

		if ((JGetUserNotification())->AskUserYes(
				JGetString(triedWrite ?
					"TriedWriteSaveNewFile::JXFileDocument" : "NoTryWriteSaveNewFile::JXFileDocument")))
			{
			return SaveInNewFile();
			}
		else
			{
			return kJFalse;
			}
		}
	else if (!exists && !JDirectoryExists(itsFilePath))
		{
		(JGetUserNotification())->ReportError(
			JGetString("DirNonexistentNoSaveError::JXFileDocument"));
		return kJFalse;
		}
	else if (!exists && !dirWritable)
		{
		(JGetUserNotification())->ReportError(
			JGetString("DirWriteProtectNoSaveError::JXFileDocument"));
		return kJFalse;
		}
	else
		{
		if (itsCheckModTimeFlag && FileModifiedByOthers())
			{
			const JUtf8Byte* map[] =
				{
				"name", itsFileName.GetBytes()
				};
			const JString msg = JGetString("OverwriteChangedFilePrompt::JXFileDocument",
										   map, sizeof(map));
			if (!(JGetUserNotification())->AskUserNo(msg))
				{
				return kJFalse;
				}
			}

		mode_t filePerms = 0;
		uid_t ownerID    = (uid_t) -1;
		gid_t groupID    = (gid_t) -1;
		if (itsMakeBackupFileFlag)
			{
			const JString backupName  = fullName + kBackupFileSuffix;
			const JBoolean makeBackup = JConvertToBoolean(
				!JFileExists(backupName) ||
				(itsIsFirstSaveFlag && itsWantNewBackupEveryOpenFlag) );
			if (makeBackup && dirWritable)
				{
				JRemoveFile(backupName);
				const JError err = JRenameFile(fullName, backupName);
				if (err.OK())
					{
					JGetPermissions(backupName, &filePerms);
					JGetOwnerID(backupName, &ownerID);
					JGetOwnerGroup(backupName, &groupID);
					}
				else
					{
					const JUtf8Byte* map[] =
						{
						"err", err.GetMessage().GetBytes()
						};
					const JString msg = JGetString("NoBackupError::JXFileDocument", map, sizeof(map));
					if (!(JGetUserNotification())->AskUserNo(msg))
						{
						return kJFalse;
						}
					}
				}
			else if (makeBackup &&
					 !(JGetUserNotification())->AskUserNo(
							JGetString("NoBackupDirWriteProtectError::JXFileDocument")))
				{
				return kJFalse;
				}
			}
		itsMakeBackupFileFlag = kJFalse;

		const JError err = WriteFile(fullName, kJFalse);
		if (err.OK())
			{
			itsSavedFlag       = kJTrue;
			itsIsFirstSaveFlag = kJFalse;
			RemoveSafetySaveFile();
			AdjustWindowTitle();

			if (filePerms != 0)
				{
				const JError err = JSetPermissions(fullName, filePerms);
				if (!err.OK())
					{
					(JGetStringManager())->ReportError("NoRestoreFilePermsError::JXFileDocument", err);
					}
				}

			if (ownerID != (uid_t) -1 || groupID != (gid_t) -1)
				{
				const JError err = JSetOwner(fullName, ownerID, groupID);
				if (!err.OK())
					{
					(JGetStringManager())->ReportError("NoRestoreFileOwnerError::JXFileDocument", err);
					}
				}

			itsCheckModTimeFlag = JGetModificationTime(fullName, &itsFileModTime).OK();
			itsCheckPermsFlag   = JGetPermissions(fullName, &itsFilePerms).OK();
			return kJTrue;
			}
		else
			{
			err.ReportIfError();
			return kJFalse;
			}
		}
}

/******************************************************************************
 WriteFile (virtual protected)

	This must be overridden if there is more than one file, or the file
	is not a plain text file.

 ******************************************************************************/

JError
JXFileDocument::WriteFile
	(
	const JString&	fullName,
	const JBoolean	safetySave
	)
	const
{
	std::ofstream output(fullName.GetBytes());
	if (output.good())
		{
		WriteTextFile(output, safetySave);

		if (output.good())
			{
			return JNoError();
			}
		else
			{
			output.close();
			JRemoveFile(fullName);
			return WriteFailed();
			}
		}
	else
		{
		// use JFOpen() to get an error message

		FILE* file = nullptr;
		JError err = JFOpen(fullName, "w", &file);
		if (file != nullptr)
			{
			// This should never happen, but who knows??

			fclose(file);
			err = JAccessDenied(fullName);
			}
		return err;
		}
}

/******************************************************************************
 WriteTextFile (virtual protected)

	This must be overridden if WriteFile() is not overridden.

 ******************************************************************************/

void
JXFileDocument::WriteTextFile
	(
	std::ostream&		output,
	const JBoolean	safetySave
	)
	const
{
	assert_msg( 0, "The programmer forgot to override JXFileDocument::WriteTextFile()" );
}

/******************************************************************************
 SafetySave (virtual)

	Save the data in a temporary file, in case the program crashes.
	No error reporting is allowed because one possibility is
	that the GUI itself has died.

 ******************************************************************************/

void
JXFileDocument::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	const JBoolean quitting = JConvertToBoolean( reason != JXDocumentManager::kTimer );

	JString homeDir;
	if ((!itsSavedFlag || (quitting && itsAutosaveBeforeCloseFlag)) &&
		(itsNeedSafetySaveFlag || quitting) &&
		(itsWasOnDiskFlag || (quitting && JGetHomeDirectory(&homeDir))))
		{
		JString fullName;

		JError err = JNoError();

		if (itsWasOnDiskFlag)
			{
			fullName = itsFilePath + itsFileName;

			fullName = itsFilePath;
			if (reason == JXDocumentManager::kAssertFired)
				{
				fullName += kAssertSavePrefix + itsFileName + kAssertSaveSuffix;
				}
			else if (itsAutosaveBeforeCloseFlag)
				{
				fullName += itsFileName;
				}
			else
				{
				fullName += kSafetySavePrefix + itsFileName + kSafetySaveSuffix;
				}
			}
		else if (reason == JXDocumentManager::kAssertFired)
			{
			err = JCreateTempFile(&homeDir, &JGetString("AssertUnsavedFilePrefix::JXFileDocument"), &fullName);
			}
		else
			{
			err = JCreateTempFile(&homeDir, &JGetString("UnsavedFilePrefix::JXFileDocument"), &fullName);
			}

		if (err.OK())
			{
			err = WriteFile(fullName, kJTrue);
			if (err.OK())
				{
				itsNeedSafetySaveFlag = kJFalse;
				if (itsSafetySaveFileName == nullptr)
					{
					itsSafetySaveFileName = jnew JString(fullName);
					assert( itsSafetySaveFileName != nullptr );
					}
				else
					{
					*itsSafetySaveFileName = fullName;
					}
				}
			}
		}

	// check if changed on disk

	if (reason == JXDocumentManager::kTimer)
		{
		CheckIfModifiedByOthers();
		}
}

/******************************************************************************
 RemoveSafetySaveFile (private)

 ******************************************************************************/

void
JXFileDocument::RemoveSafetySaveFile()
{
	if (itsSafetySaveFileName != nullptr)
		{
		JRemoveFile(*itsSafetySaveFileName);

		jdelete itsSafetySaveFileName;
		itsSafetySaveFileName = nullptr;
		}

	itsNeedSafetySaveFlag = kJFalse;
}

/******************************************************************************
 GetSafetySaveFileName

	Returns kJTrue if the file has been safety saved.

 ******************************************************************************/

JBoolean
JXFileDocument::GetSafetySaveFileName
	(
	JString* fileName
	)
	const
{
	if (itsSafetySaveFileName != nullptr)
		{
		*fileName = *itsSafetySaveFileName;
		return kJTrue;
		}
	else
		{
		fileName->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 CheckForSafetySaveFiles (static)

	Checks for the existence of a safety save version with a later
	modification date.  If it exists, asks the user if it should be
	opened.  *filesToOpen contains the names of these files, if any.

 ******************************************************************************/

JBoolean
JXFileDocument::CheckForSafetySaveFiles
	(
	const JString&		origFullName,
	JPtrArray<JString>*	filesToOpen
	)
{
	filesToOpen->CleanOut();

	JString fullName, path, name;
	if (!JGetTrueName(origFullName, &fullName))
		{
		return kJFalse;
		}
	JSplitPathAndName(fullName, &path, &name);

	time_t modTime, safetyTime, assertTime;
	if (!name.BeginsWith(kSafetySavePrefix) &&
		JGetModificationTime(fullName, &modTime) == kJNoError)
		{
		const JString safetyName = path + kSafetySavePrefix + name + kSafetySaveSuffix;
		const JBoolean safetyExists = JConvertToBoolean(
			JGetModificationTime(safetyName, &safetyTime) == kJNoError &&
			safetyTime > modTime);

		const JString assertName = path + kAssertSavePrefix + name + kAssertSaveSuffix;
		const JBoolean assertExists = JConvertToBoolean(
			JGetModificationTime(assertName, &assertTime) == kJNoError &&
			assertTime > modTime);

		const JUtf8Byte* id = nullptr;
		if (safetyExists && assertExists)
			{
			id = "OpenSafetyAssertFilePrompt::JXFileDocument";
			}
		else if (safetyExists)
			{
			id = "OpenSafetyFilePrompt::JXFileDocument";
			}
		else if (assertExists)
			{
			id = "OpenAssertFilePrompt::JXFileDocument";
			}

		const JUtf8Byte* map[] =
			{
			"name", name.GetBytes()
			};

		if (id != nullptr &&
			(JGetUserNotification())->AskUserYes(
				JGetString(id, map, sizeof(map))))
			{
			if (safetyExists)
				{
				JString* s = jnew JString(safetyName);
				assert( s != nullptr );
				filesToOpen->Append(s);
				}
			if (assertExists)
				{
				JString* s = jnew JString(assertName);
				assert( s != nullptr );
				filesToOpen->Append(s);
				}
			}
		}

	return !filesToOpen->IsEmpty();
}

/******************************************************************************
 DefaultCanReadASCIIFile (static protected)

	We provide this as a convenience for simple derived classes.
	It assumes that the front of the file is {signature, version} and that
	all older versions are readable.  It returns the actual file version
	so the caller can perform further checks if necessary.

 ******************************************************************************/

JXFileDocument::FileStatus
JXFileDocument::DefaultCanReadASCIIFile
	(
	std::istream&		input,
	const JUtf8Byte*	fileSignature,
	const JFileVersion	latestFileVersion,
	JFileVersion*		actualFileVersion
	)
{
	const JSize offset = JTellg(input);

	const JString filePrefix = JRead(input, strlen(fileSignature));
	if (filePrefix != fileSignature)
		{
		JSeekg(input, offset);
		*actualFileVersion = 0;
		return kNotMyFile;
		}

	input >> *actualFileVersion;
	JSeekg(input, offset);

	if (*actualFileVersion <= latestFileVersion)
		{
		return kFileReadable;
		}
	else
		{
		return kNeedNewerVersion;
		}
}

/******************************************************************************
 AdjustWindowTitle (protected)

	We prepend spaces to the names of documents that don't need to be
	saved so fvwm will allocate enough width in the iconified state
	to have space for the asterisks when they appear later.

 ******************************************************************************/

static const JUtf8Byte* kNeedsSavePrefixStr      = "*** ";
static const JUtf8Byte* kNeedsSavePlaceholderStr = "     ";

void
JXFileDocument::AdjustWindowTitle()
{
	JXWindow* window = GetWindow();
	if (window != nullptr)
		{
		JString title = GetWindowTitle();
		if (!itsSavedFlag)
			{
			title.Prepend(kNeedsSavePrefixStr);
			}
		else if (itsAllocateTitleSpaceFlag)
			{
			title.Prepend(kNeedsSavePlaceholderStr);
			}
		window->SetTitle(title);
		}
}

/******************************************************************************
 SkipNeedsSavePrefix (static)

 ******************************************************************************/

const JUtf8Byte*
JXFileDocument::SkipNeedsSavePrefix
	(
	const JUtf8Byte* s
	)
{
	JSize len = strlen(kNeedsSavePrefixStr);
	if (JString::CompareMaxNBytes(s, kNeedsSavePrefixStr, len) == 0)
		{
		return s + len;
		}

	len = strlen(kNeedsSavePlaceholderStr);
	if (JString::CompareMaxNBytes(s, kNeedsSavePlaceholderStr, len) == 0)
		{
		return s + len;
		}

	return s;
}

/******************************************************************************
 GetWindowTitle (virtual protected)

	This is virtual so derived classes can adjust the window's title.
	This should be overridden in conjunction with GetName().

 ******************************************************************************/

JString
JXFileDocument::GetWindowTitle()
	const
{
	return GetName();
}

/******************************************************************************
 Prompts

 ******************************************************************************/

void
JXFileDocument::SetSaveBeforeClosePrompt
	(
	const JString& prompt
	)
{
	itsSaveBeforeClosePrompt = prompt;
}

void
JXFileDocument::SetSaveNewFilePrompt
	(
	const JString& prompt
	)
{
	itsSaveNewFilePrompt = prompt;
}

void
JXFileDocument::SetOKToRevertPrompt
	(
	const JString& prompt
	)
{
	itsOKToRevertPrompt = prompt;
}
