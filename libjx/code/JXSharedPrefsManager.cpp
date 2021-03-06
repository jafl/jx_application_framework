/******************************************************************************
 JXSharedPrefsManager.cpp

	Stores preferences that are shared by all JX programs.  Since this must
	be compatible with all versions of JX, all older versions of the data
	must be written along with the latest version.  Programs that use an
	outdated version of JX cannot usually write to this file because they
	are unable to update all the relevant information.

	For single items like PartialWordModifier, the best method is to create
	another ID when values are added to the enum.  When the new version opens
	the file for the first time, it reads the original ID and creates the
	new one.  Old programs can still read and write the original ID, while
	new programs read and write the new ID.  (Old programs cannot read the
	new ID since it has illegal values.)

	Since JFileArray modifies the file every time it is opened, we use
	a separate signal file to notify other running programs when the settings
	change.  We use an idle task to check for signals from other programs.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXSharedPrefsManager.h>
#include <JXSharedPrefObject.h>
#include <JPrefsFile.h>
#include <JXSaveFileInput.h>
#include <JXWindow.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kDataFileRoot   = "jx/globals";
static const JCharacter* kSignalFileName = "~/.jx/globals.signal";

const JSize kUpdateInterval = 1000;		// milliseconds

// JBroadcaster message types

const JCharacter* JXSharedPrefsManager::kRead = "Read::JXSharedPrefsManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSharedPrefsManager::JXSharedPrefsManager()
{
	itsFile = NULL;

	if (JExpandHomeDirShortcut(kSignalFileName, &itsSignalFileName))
		{
		JString path, name;
		JSplitPathAndName(itsSignalFileName, &path, &name);
		if (JCreateDirectory(path, 0700) != kJNoError)
			{
			itsSignalFileName.Clear();
			}
		}
	else
		{
		itsSignalFileName.Clear();
		}

	GetAll(&itsWasNewFlag);

	itsUpdateTask = jnew JXTimerTask(kUpdateInterval);
	assert( itsUpdateTask != NULL );
	itsUpdateTask->Start();
	ListenTo(itsUpdateTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSharedPrefsManager::~JXSharedPrefsManager()
{
	jdelete itsFile;
//	jdelete itsUpdateTask;	// already deleted by JXApplication!
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXSharedPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsUpdateTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		Update();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 Update (private)

	Returns kJTrue if the settings were read in.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::Update()
{
	time_t t;
	if (!itsSignalFileName.IsEmpty() &&
		JGetModificationTime(itsSignalFileName, &t) == kJNoError &&
		t != itsSignalModTime)
		{
		JBoolean isNew;
		return GetAll(&isNew);
		}
	else
		{
		// This exists because JCore values cannot update themsevles,
		// and it seems easier to toss them all in here.

		if ((itsOrigFocusInDockFlag    != JXWindow::WillFocusFollowCursorInDock() ||
			 itsOrigCopyWhenSelectFlag != JTextEditor::WillCopyWhenSelect()       ||
			 itsOrigMiddleClickWillPasteFlag != JXTEBase::MiddleButtonWillPaste() ||
			 itsOrigPWMod              != JXTEBase::GetPartialWordModifier()      ||
			 itsOrigCaretScrollFlag    != JXTEBase::CaretWillFollowScroll()       ||
			 itsOrigWindowsHomeEndFlag != JXTEBase::WillUseWindowsHomeEnd()       ||
			 itsOrigAllowSpaceFlag     != JXSaveFileInput::WillAllowSpace()       ||
			 itsOrigMenuDisplayStyle   != JXMenu::GetDisplayStyle()) &&
			Open())
			{
			PrivateSetFocusFollowsCursorInDock();
			PrivateSetCopyWhenSelectFlag();
			PrivateSetMiddleClickWillPasteFlag();
			PrivateSetPartialWordModifier();
			PrivateSetCaretFollowsScroll();
			PrivateSetWindowsHomeEnd();
			PrivateSetAllowSpaceFlag();
			PrivateSetMenuDisplayStyle();
			Close(kJTrue);
			}

		return kJFalse;
		}
}

/******************************************************************************
 GetAll (private)

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::GetAll
	(
	JBoolean* isNew
	)
{
	if (!Open())
		{
		*isNew = kJTrue;
		return kJFalse;
		}

	*isNew = itsFile->IsEmpty();

	JBoolean changed = kJFalse;
	std::string data;

	if (itsFile->IDValid(kFocusFollowsCursorInDockID))
		{
		itsFile->GetData(kFocusFollowsCursorInDockID, &data);
		std::istringstream input(data);
		input >> itsOrigFocusInDockFlag;
		JXWindow::ShouldFocusFollowCursorInDock(itsOrigFocusInDockFlag);
		}
	else
		{
		PrivateSetFocusFollowsCursorInDock();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kCopyWhenSelectID))
		{
		itsFile->GetData(kCopyWhenSelectID, &data);
		std::istringstream input(data);
		input >> itsOrigCopyWhenSelectFlag;
		JTextEditor::ShouldCopyWhenSelect(itsOrigCopyWhenSelectFlag);
		}
	else
		{
		PrivateSetCopyWhenSelectFlag();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kMiddleClickWillPasteID))
		{
		itsFile->GetData(kMiddleClickWillPasteID, &data);
		std::istringstream input(data);
		input >> itsOrigMiddleClickWillPasteFlag;
		JXTEBase::MiddleButtonShouldPaste(itsOrigMiddleClickWillPasteFlag);
		}
	else
		{
		PrivateSetMiddleClickWillPasteFlag();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kPWModifierID))
		{
		itsFile->GetData(kPWModifierID, &data);
		std::istringstream input(data);
		input >> itsOrigPWMod;
		JXTEBase::SetPartialWordModifier(itsOrigPWMod);
		}
	else
		{
		PrivateSetPartialWordModifier();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kCaretFollowsScrollID))
		{
		itsFile->GetData(kCaretFollowsScrollID, &data);
		std::istringstream input(data);
		input >> itsOrigCaretScrollFlag;
		JXTEBase::CaretShouldFollowScroll(itsOrigCaretScrollFlag);
		}
	else
		{
		PrivateSetCaretFollowsScroll();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kWindowsHomeEndID))
		{
		itsFile->GetData(kWindowsHomeEndID, &data);
		std::istringstream input(data);
		input >> itsOrigWindowsHomeEndFlag;
		JXTEBase::ShouldUseWindowsHomeEnd(itsOrigWindowsHomeEndFlag);
		}
	else
		{
		if (!(*isNew))
			{
			JXTEBase::ShouldUseWindowsHomeEnd(kJFalse);
			}

		PrivateSetWindowsHomeEnd();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kAllowSpaceID))
		{
		itsFile->GetData(kAllowSpaceID, &data);
		std::istringstream input(data);
		input >> itsOrigAllowSpaceFlag;
		JXSaveFileInput::ShouldAllowSpace(itsOrigAllowSpaceFlag);
		}
	else
		{
		PrivateSetAllowSpaceFlag();
		changed = kJTrue;
		}

	if (itsFile->IDValid(kMenuDisplayStyleID))
		{
		itsFile->GetData(kMenuDisplayStyleID, &data);
		std::istringstream input(data);
		long temp;
		input >> temp;
		itsOrigMenuDisplayStyle = (JXMenu::Style) temp;
		if (itsOrigMenuDisplayStyle > JXMenu::kStyleMax)
			{
			itsOrigMenuDisplayStyle = JXMenu::kWindowsStyle;
			}
		JXMenu::SetDisplayStyle(itsOrigMenuDisplayStyle);
		}
	else
		{
		if (!(*isNew))
			{
			JXMenu::SetDisplayStyle(JXMenu::kMacintoshStyle);
			}

		PrivateSetMenuDisplayStyle();
		changed = kJTrue;
		}

	Broadcast(Read());

	Close(changed);
	SaveSignalModTime();
	return kJTrue;
}

/******************************************************************************
 PrivateSetFocusFollowsCursorInDock (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetFocusFollowsCursorInDock()
{
	assert( itsFile != NULL );

	itsOrigFocusInDockFlag = JXWindow::WillFocusFollowCursorInDock();

	std::ostringstream data;
	data << itsOrigFocusInDockFlag;
	itsFile->SetData(kFocusFollowsCursorInDockID, data);
}

/******************************************************************************
 PrivateSetCopyWhenSelectFlag (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetCopyWhenSelectFlag()
{
	assert( itsFile != NULL );

	itsOrigCopyWhenSelectFlag = JTextEditor::WillCopyWhenSelect();

	std::ostringstream data;
	data << itsOrigCopyWhenSelectFlag;
	itsFile->SetData(kCopyWhenSelectID, data);
}

/******************************************************************************
 PrivateSetMiddleClickWillPasteFlag (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetMiddleClickWillPasteFlag()
{
	assert( itsFile != NULL );

	itsOrigMiddleClickWillPasteFlag = JXTEBase::MiddleButtonWillPaste();

	std::ostringstream data;
	data << itsOrigMiddleClickWillPasteFlag;
	itsFile->SetData(kMiddleClickWillPasteID, data);
}

/******************************************************************************
 PrivateSetPartialWordModifier (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetPartialWordModifier()
{
	assert( itsFile != NULL );

	itsOrigPWMod = JXTEBase::GetPartialWordModifier();

	std::ostringstream data;
	data << itsOrigPWMod;
	itsFile->SetData(kPWModifierID, data);
}

/******************************************************************************
 PrivateSetCaretFollowsScroll (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetCaretFollowsScroll()
{
	assert( itsFile != NULL );

	itsOrigCaretScrollFlag = JXTEBase::CaretWillFollowScroll();

	std::ostringstream data;
	data << itsOrigCaretScrollFlag;
	itsFile->SetData(kCaretFollowsScrollID, data);
}

/******************************************************************************
 PrivateSetWindowsHomeEnd (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetWindowsHomeEnd()
{
	assert( itsFile != NULL );

	itsOrigWindowsHomeEndFlag = JXTEBase::WillUseWindowsHomeEnd();

	std::ostringstream data;
	data << itsOrigWindowsHomeEndFlag;
	itsFile->SetData(kWindowsHomeEndID, data);
}

/******************************************************************************
 PrivateSetAllowSpaceFlag (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetAllowSpaceFlag()
{
	assert( itsFile != NULL );

	itsOrigAllowSpaceFlag = JXSaveFileInput::WillAllowSpace();

	std::ostringstream data;
	data << itsOrigAllowSpaceFlag;
	itsFile->SetData(kAllowSpaceID, data);
}

/******************************************************************************
 PrivateSetMenuDisplayStyle (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetMenuDisplayStyle()
{
	assert( itsFile != NULL );

	itsOrigMenuDisplayStyle = JXMenu::GetDisplayStyle();

	std::ostringstream data;
	data << (long) itsOrigMenuDisplayStyle;
	itsFile->SetData(kMenuDisplayStyleID, data);
}

/******************************************************************************
 ReadPrefs

 ******************************************************************************/

void
JXSharedPrefsManager::ReadPrefs
	(
	JXSharedPrefObject* obj
	)
{
	if (itsFile != NULL)
		{
		if (PrivateReadPrefs(obj))
			{
			itsChangedFlag = kJTrue;
			}
		}
	else if (!Update() && Open())
		{
		const JBoolean changed = PrivateReadPrefs(obj);
		Close(changed);
		}
}

/******************************************************************************
 PrivateReadPrefs (private)

	Returns kJTrue if anything changed.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::PrivateReadPrefs
	(
	JXSharedPrefObject* obj
	)
{
	assert( itsFile != NULL );

	const JPrefID& latestVersID = obj->GetLatestVersionID();
	if (itsFile->IDValid(latestVersID))
		{
		std::string data;
		itsFile->GetData(latestVersID, &data);
		std::istringstream versInput(data);
		JFileVersion vers;
		versInput >> vers;

		const JPrefID id = obj->GetPrefID(vers);
		itsFile->GetData(id, &data);

		std::istringstream input(data);
		obj->ReadPrefs(input);

		return kJFalse;
		}
	else
		{
		return PrivateWritePrefs(obj);
		}
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
JXSharedPrefsManager::WritePrefs
	(
	const JXSharedPrefObject* obj
	)
{
	if (itsFile != NULL)
		{
		if (PrivateWritePrefs(obj))
			{
			itsChangedFlag = kJTrue;
			}
		}
	else if (Open())
		{
		const JBoolean changed = PrivateWritePrefs(obj);
		Close(changed);
		}
}

/******************************************************************************
 PrivateWritePrefs (private)

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::PrivateWritePrefs
	(
	const JXSharedPrefObject* obj
	)
{
	assert( itsFile != NULL );

	const JFileVersion currVers = obj->GetCurrentPrefsVersion();
	const JPrefID& latestVersID = obj->GetLatestVersionID();

	if (itsFile->IDValid(latestVersID))
		{
		std::string data;
		itsFile->GetData(latestVersID, &data);
		std::istringstream input(data);
		JFileVersion origVers;
		input >> origVers;
		if (origVers > currVers)
			{
			return kJFalse;
			}
		}

	std::ostringstream versData;
	versData << currVers;
	itsFile->SetData(latestVersID, versData);

	const JArray<JXSharedPrefObject::VersionInfo> versList = obj->GetVersionList();

	const JSize versCount = versList.GetElementCount();
	for (JIndex i=1; i<=versCount; i++)
		{
		JXSharedPrefObject::VersionInfo info = versList.GetElement(i);

		std::ostringstream data;
		obj->WritePrefs(data, info.vers);
		itsFile->SetData(info.id, data);
		}

	return kJTrue;
}

/******************************************************************************
 Open (private)

	This code suffers from a race condition.  If it ever becomes a problem,
	JFileArray will need to use lockfile instead.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::Open()
{
	assert( itsFile == NULL );

	const JError err = JPrefsFile::Create(kDataFileRoot, &itsFile,
										  JFileArray::kDeleteIfWaitTimeout);
	itsChangedFlag = kJFalse;
	return err.OK();
}

/******************************************************************************
 Close (private)

 ******************************************************************************/

void
JXSharedPrefsManager::Close
	(
	const JBoolean changed
	)
{
	jdelete itsFile;
	itsFile = NULL;

	if (changed || itsChangedFlag)
		{
		NotifyChanged();
		}
}

/******************************************************************************
 NotifyChanged (private)

 ******************************************************************************/

void
JXSharedPrefsManager::NotifyChanged()
{
	if (!itsSignalFileName.IsEmpty())
		{
		std::ofstream temp(itsSignalFileName);
		temp << 'x';
		temp.close();

		SaveSignalModTime();
		}
}

/******************************************************************************
 SaveSignalModTime (private)

 ******************************************************************************/

void
JXSharedPrefsManager::SaveSignalModTime()
{
	if (!itsSignalFileName.IsEmpty())
		{
		JGetModificationTime(itsSignalFileName, &itsSignalModTime);
		}
}
