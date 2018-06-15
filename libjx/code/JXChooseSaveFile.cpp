/******************************************************************************
 JXChooseSaveFile.cpp

	JX class to let user easily select and save files.

	BASE CLASS = JChooseSaveFile, JPrefObject, virtual JBroadcaster

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#include "JXChooseSaveFile.h"
#include "JXChooseFileDialog.h"
#include "JXChoosePathDialog.h"
#include "JXSaveFileDialog.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <sstream>
#include <jAssert.h>

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseSaveFile::JXChooseSaveFile
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
	:
	JChooseSaveFile(),
	JPrefObject(prefsMgr, id)
{
	itsDirInfo           = nullptr;	// constructed in GetDirInfo()
	itsCurrentDialog     = nullptr;
	itsChooseFileDialog  = nullptr;
	itsChoosePathDialog  = nullptr;
	itsSaveFileDialog    = nullptr;

	itsResultStr  = nullptr;
	itsResultList = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseSaveFile::~JXChooseSaveFile()
{
	jdelete itsDirInfo;
}

/******************************************************************************
 ChooseFile

	Displays the prompt and asks the user to choose a file.

	Returns kJFalse if user cancels.

	To change which files are display, override SetChooseFileFilters().

 ******************************************************************************/

JBoolean
JXChooseSaveFile::ChooseFile
	(
	const JString&	prompt,
	const JString&	instructions,
	JString*		fullName
	)
{
	itsResultStr = fullName;
	return ChooseFile(prompt, instructions, JString::empty, kJFalse);
}

JBoolean
JXChooseSaveFile::ChooseFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origName,
	JString*		fullName
	)
{
	itsResultStr = fullName;
	return ChooseFile(prompt, instructions, origName, kJFalse);
}

JBoolean
JXChooseSaveFile::ChooseFiles
	(
	const JString&		prompt,
	const JString&		instructions,
	JPtrArray<JString>*	fullNameList
	)
{
	itsResultList = fullNameList;
	return ChooseFile(prompt, instructions, JString::empty, kJTrue);
}

JBoolean
JXChooseSaveFile::ChooseFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	wildcardFilter,
	const JString&	origName,
	JString*		fullName
	)
{
	const JString origFilter = itsUserFilter;
	itsUserFilter            = wildcardFilter;

	itsResultStr      = fullName;
	const JBoolean ok = ChooseFile(prompt, instructions, origName, kJFalse);

	itsUserFilter = origFilter;
	return ok;
}

JBoolean
JXChooseSaveFile::ChooseFiles
	(
	const JString&		prompt,
	const JString&		instructions,
	const JString&		wildcardFilter,
	JPtrArray<JString>*	fullNameList
	)
{
	const JString origFilter = itsUserFilter;
	itsUserFilter            = wildcardFilter;

	itsResultList     = fullNameList;
	const JBoolean ok = ChooseFile(prompt, instructions, JString::empty, kJTrue);

	itsUserFilter = origFilter;
	return ok;
}

// private

JBoolean
JXChooseSaveFile::ChooseFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	originalName,
	const JBoolean	allowSelectMultiple
	)
{
	JDirInfo* dirInfo       = GetDirInfo();
	JBoolean restorePath    = kJFalse;
	const JString savedPath = dirInfo->GetDirectory();

	JString origName;
	if (originalName.Contains(ACE_DIRECTORY_SEPARATOR_STR))
		{
		JString path;
		JSplitPathAndName(originalName, &path, &origName);
		dirInfo->GoToClosest(path);
		restorePath = kJTrue;
		if (!JSameDirEntry(path, dirInfo->GetDirectory()))
			{
			origName.Clear();
			}
		}

	assert( itsChooseFileDialog == nullptr );

	(JXGetApplication())->PrepareForBlockingWindow();

	itsChooseFileDialog =
		CreateChooseFileDialog(JXGetApplication(), dirInfo,
							   itsUserFilter, allowSelectMultiple,
							   origName, instructions);

	SetChooseFileFilters(dirInfo);
	RestoreState(itsChooseFileDialog, restorePath);
	WaitForResponse(itsChooseFileDialog);

	if (restorePath)
		{
		dirInfo->GoTo(savedPath);
		}

	itsChooseFileDialog = nullptr;
	return itsResponse;
}

// virtual protected

JXChooseFileDialog*
JXChooseSaveFile::CreateChooseFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JBoolean	allowSelectMultiple,
	const JString&	origName,
	const JString&	message
	)
{
	return JXChooseFileDialog::Create(supervisor, dirInfo, fileFilter, allowSelectMultiple,
									  origName, message);
}

/******************************************************************************
 SetChooseFileFilters (virtual protected)

	Called before Choose File dialog is displayed.  This provides the
	opportunity to change what is displayed or set a content filter.

 ******************************************************************************/

void
JXChooseSaveFile::SetChooseFileFilters
	(
	JDirInfo* dirInfo
	)
{
	dirInfo->ResetCSFFilters();
}

/******************************************************************************
 ChooseRPath

	Displays the prompt and asks the user to choose a readable path.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JXChooseSaveFile::ChooseRPath
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	return ChoosePath(kJFalse, instructions, origPath, newPath);
}

/******************************************************************************
 ChooseRWPath

	Displays the prompt and asks the user to choose a writable path.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JXChooseSaveFile::ChooseRWPath
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	return ChoosePath(kJTrue, instructions, origPath, newPath);
}

/******************************************************************************
 ChoosePath (private)

	origPath can be nullptr.

 ******************************************************************************/

JBoolean
JXChooseSaveFile::ChoosePath
	(
	const JBoolean	selectOnlyWritable,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	itsResultStr = newPath;

	JDirInfo* dirInfo       = GetDirInfo();
	JBoolean restorePath    = kJFalse;
	const JString savedPath = dirInfo->GetDirectory();

	if (!origPath.IsEmpty())
		{
		dirInfo->GoToClosest(origPath);
		restorePath = kJTrue;
		}

	assert( itsChoosePathDialog == nullptr );

	(JXGetApplication())->PrepareForBlockingWindow();

	itsChoosePathDialog =
		CreateChoosePathDialog(JXGetApplication(), dirInfo, itsUserFilter,
							   selectOnlyWritable, instructions);

	dirInfo->ResetCSFFilters();
	RestoreState(itsChoosePathDialog, restorePath);
	WaitForResponse(itsChoosePathDialog);

	if (restorePath)
		{
		dirInfo->GoTo(savedPath);
		}

	itsChoosePathDialog = nullptr;
	return itsResponse;
}

// virtual protected

JXChoosePathDialog*
JXChooseSaveFile::CreateChoosePathDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JBoolean	selectOnlyWritable,
	const JString&	message
	)
{
	return JXChoosePathDialog::Create(supervisor, dirInfo, fileFilter,
									  selectOnlyWritable, message);
}

/******************************************************************************
 SaveFile

	Displays the prompts and asks the user for a file name.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JXChooseSaveFile::SaveFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	originalName,
	JString*		newFullName
	)
{
	itsResultStr = newFullName;

	JDirInfo* dirInfo       = GetDirInfo();
	JBoolean restorePath    = kJFalse;
	const JString savedPath = dirInfo->GetDirectory();

	JString origName = originalName;
	if (origName.Contains(ACE_DIRECTORY_SEPARATOR_STR))
		{
		JString path;
		JSplitPathAndName(originalName, &path, &origName);
		dirInfo->GoToClosest(path);
		restorePath = kJTrue;
		}

	assert( itsSaveFileDialog == nullptr );

	(JXGetApplication())->PrepareForBlockingWindow();

	itsSaveFileDialog =
		CreateSaveFileDialog(JXGetApplication(), dirInfo, itsUserFilter,
							 origName, prompt, instructions);

	dirInfo->ResetCSFFilters();
	RestoreState(itsSaveFileDialog, restorePath);
	WaitForResponse(itsSaveFileDialog);

	if (restorePath)
		{
		dirInfo->GoTo(savedPath);
		}

	itsSaveFileDialog = nullptr;
	return itsResponse;
}

// virtual protected

JXSaveFileDialog*
JXChooseSaveFile::CreateSaveFileDialog
	(
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JString&	origName,
	const JString&	prompt,
	const JString&	message
	)
{
	return JXSaveFileDialog::Create(supervisor, dirInfo, fileFilter,
									origName, prompt, message);
}

/******************************************************************************
 Receive (protected)

	Listen for response from itsCurrentDialog.

 ******************************************************************************/

void
JXChooseSaveFile::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCurrentDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		itsResponse   = info->Successful();
		itsUserFilter = itsCurrentDialog->GetFilter();
		SaveState(itsCurrentDialog);

		if (itsResponse && sender == itsChooseFileDialog && itsResultList != nullptr)
			{
			const JBoolean ok = itsChooseFileDialog->GetFullNames(itsResultList);
			assert( ok );
			}
		else if (itsResponse && sender == itsChooseFileDialog)
			{
			assert( itsResultStr != nullptr );
			const JBoolean ok = itsChooseFileDialog->GetFullName(itsResultStr);
			assert( ok );
			}
		else if (itsResponse && sender == itsChoosePathDialog)
			{
			assert( itsResultStr != nullptr );
			*itsResultStr = itsChoosePathDialog->GetPath();
			}
		else if (itsResponse && sender == itsSaveFileDialog)
			{
			assert( itsResultStr != nullptr );
			JString name;
			const JBoolean ok = itsSaveFileDialog->GetFileName(&name);
			assert( ok );
			*itsResultStr = JCombinePathAndName(itsSaveFileDialog->GetPath(), name);
			}
		else if (itsResultStr != nullptr)
			{
			itsResultStr->Clear();
			}
		else if (itsResultList != nullptr)
			{
			itsResultList->CleanOut();
			}
		itsCurrentDialog = nullptr;
		itsResultStr     = nullptr;
		itsResultList    = nullptr;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 WaitForResponse (private)

	Caller must call app->PrepareForBlockingWindow() before creating window.

 ******************************************************************************/

void
JXChooseSaveFile::WaitForResponse
	(
	JXCSFDialogBase* dlog
	)
{
	assert( itsCurrentDialog == nullptr );

	itsCurrentDialog = dlog;
	ListenTo(itsCurrentDialog);
	itsCurrentDialog->BeginDialog();

	// display the inactive cursor in all the other windows

	JXApplication* app = JXGetApplication();
	app->DisplayInactiveCursor();

	// block with event loop running until we get a response

	JXWindow* window = itsCurrentDialog->GetWindow();
	while (itsCurrentDialog != nullptr)
		{
		app->HandleOneEventForWindow(window);
		}

	app->BlockingWindowFinished();
}

/******************************************************************************
 RestoreState (private)

	Let the new dialog read its state from our saved data.

 ******************************************************************************/

void
JXChooseSaveFile::RestoreState
	(
	JXCSFDialogBase*	dlog,
	const JBoolean		ignoreScroll
	)
{
	(dlog->GetWindow())->PlaceAsDialogWindow();

	JString* dialogState = GetDialogState();
	if (!dialogState->IsEmpty())
		{
		const std::string s(dialogState->GetBytes(), dialogState->GetByteCount());
		std::istringstream input(s);
		dlog->ReadBaseSetup(input, ignoreScroll);
		}
}

/******************************************************************************
 SaveState (private)

	Let the dialog save its state in our saved data.

 ******************************************************************************/

void
JXChooseSaveFile::SaveState
	(
	JXCSFDialogBase* dlog
	)
{
	std::ostringstream output;
	dlog->WriteBaseSetup(output);
	*(GetDialogState()) = output.str();
}

/******************************************************************************
 ReadSetup

	Read in setup information.  Derived classes of JXApplication can use
	this to restore our state from the previous time the program was run.

	We assert that we can read the data that is provided because there
	is no way to skip over it.

 ******************************************************************************/

void
JXChooseSaveFile::ReadSetup
	(
	std::istream& input
	)
{
	input >> std::ws;

	JFileVersion vers;
	if (input.peek() == '"')
		{
		vers = 0;
		}
	else
		{
		input >> vers;
		}
	assert( vers <= kCurrentSetupVersion );

	input >> *(GetDialogState());
}

/******************************************************************************
 WriteSetup

	Write setup information.  Derived classes of JXApplication can use this
	to save our state for the next time the program is run.

 ******************************************************************************/

void
JXChooseSaveFile::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << GetDialogState();
	output << ' ';
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
JXChooseSaveFile::ReadPrefs
	(
	std::istream& input
	)
{
	ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
JXChooseSaveFile::WritePrefs
	(
	std::ostream& output
	)
	const
{
	WriteSetup(output);
}

/******************************************************************************
 GetDirInfo (protected)

	Since JDirInfo requires a progress display, we can't create it until
	after -all- the globals have been initialized.  We are one of the
	globals, so we can't create it in our constructor.

	This also provides a convenient place to ensure that all other CSF
	objects use the same JDirInfo object.

 ******************************************************************************/

JDirInfo*
JXChooseSaveFile::GetDirInfo()
{
	if (itsDirInfo == nullptr && this == JXGetChooseSaveFile())
		{
		JString dirName = JGetCurrentDirectory();
		if (!JDirInfo::Create(dirName, &itsDirInfo))
			{
			JBoolean ok = JGetHomeDirectory(&dirName);
			if (!ok || !JDirInfo::Create(dirName, &itsDirInfo))
				{
				dirName = JGetRootDirectory();
				ok = JDirInfo::Create(dirName, &itsDirInfo);
				assert( ok );
				}
			}
		itsDirInfo->ShowHidden(kJFalse);
		return itsDirInfo;
		}
	else if (itsDirInfo == nullptr)
		{
		return (JXGetChooseSaveFile())->GetDirInfo();
		}
	else
		{
		return itsDirInfo;
		}
}

/******************************************************************************
 GetDialogState (private)

	Since JDirInfo requires a progress display, we can't create it until
	after -all- the globals have been initialized.  We are one of the
	globals, so we can't create it in our constructor.

	This also provides a convenient place to ensure that all other CSF
	objects use the same JDirInfo object.

 ******************************************************************************/

const JString&
JXChooseSaveFile::GetDialogState()
	const
{
	if (this == JXGetChooseSaveFile())
		{
		return itsDialogState;
		}
	else
		{
		return ((const JXChooseSaveFile*) JXGetChooseSaveFile())->GetDialogState();
		}
}

JString*
JXChooseSaveFile::GetDialogState()
{
	if (this == JXGetChooseSaveFile())
		{
		return &itsDialogState;
		}
	else
		{
		return (JXGetChooseSaveFile())->GetDialogState();
		}
}

/******************************************************************************
 IsCharacterInWord (static)

	Returns true if the given character should be considered part of
	a word.  Our definition is [A-Za-z0-9_].

 ******************************************************************************/

JBoolean
JXChooseSaveFile::IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return JI2B( c.IsAlnum() || c == '_' );
}
