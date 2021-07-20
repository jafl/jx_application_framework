/******************************************************************************
 SVNPrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "SVNPrefsManager.h"
#include "SVNPrefsDialog.h"
#include "svnGlobals.h"
#include <JXChooseSaveFile.h>
#include <JXWindow.h>
#include <jProcessUtil.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 1;

	// version  1 adds commit editor to kSVNIntegrationID.

// integration

static const JUtf8Byte* kCodeCrusaderCmd[] =
{
	"jcc --vcs-commit",
	"",		// not used
	"jcc --reload-open"
};

static const JUtf8Byte* kCmdLineCmd[] =
{
	"gnome-terminal -t \"Commit\" -x vi",
	"gnome-terminal -t \"svn diff $rev_option $file_name\" -x /bin/sh -c \"svn diff $rev_option $file_name | less\"",
	""		// not possible
};

static const JUtf8Byte** kIntegrationCmd[2] =
{
	kCodeCrusaderCmd,
	kCmdLineCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

SVNPrefsManager::SVNPrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true),
	itsPrefsDialog(nullptr)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kSVNgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SVNPrefsManager::~SVNPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
SVNPrefsManager::SaveAllBeforeDestruct()
{
	SetData(kSVNProgramVersionID, SVNGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 GetExpirationTimeStamp

 ******************************************************************************/

bool
SVNPrefsManager::GetExpirationTimeStamp
	(
	time_t* t
	)
	const
{
	std::string data;
	if (GetData(kSVNExpireTimeStampID, &data))
		{
		std::istringstream input(data);
		input >> *t;
		return !input.fail();
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 SetExpirationTimeStamp

 ******************************************************************************/

void
SVNPrefsManager::SetExpirationTimeStamp
	(
	const time_t t
	)
{
	std::ostringstream data;
	data << t;

	SetData(kSVNExpireTimeStampID, data);
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
SVNPrefsManager::UpgradeData
	(
	const bool		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(kSVNProgramVersionID, SVNGetVersionNumberStr());
		}

	if (!isNew && currentVersion < 1)
		{
		Integration type;
		JString commitEditor, diffCmd, reloadChangedCmd;

		std::string data;
		if (GetData(kSVNIntegrationID, &data))
			{
			std::istringstream dataStream(data);
			dataStream >> type;
			dataStream >> diffCmd;
			dataStream >> reloadChangedCmd;

			const JUtf8Byte* editor = getenv("SVN_EDITOR");
			if (type == kCodeCrusader)
				{
				commitEditor = kCodeCrusaderCmd[ kCommitEditor ];
				}
			else if (type == kCustom && !JString::IsEmpty(editor))
				{
				commitEditor = editor;
				}
			else	// type == kCmdLine
				{
				commitEditor = kCmdLineCmd[ kCommitEditor ];
				}

			SetIntegration(type, commitEditor, diffCmd, reloadChangedCmd);
			}
		}

	// check if Code Crusader is available

	const bool hasJCC = JProgramAvailable(JGetString("CodeCrusaderBinary::SVNGlobal"));

	Integration type;
	JString commitEditor, diffCmd, reloadChangedCmd;
	if (GetIntegration(&type, &commitEditor, &diffCmd, &reloadChangedCmd))
		{
		bool changed = false;
		if (hasJCC && type != kCodeCrusader)
			{
			type    = kCodeCrusader;
			changed = true;
			}
		else if (!hasJCC && type == kCodeCrusader)
			{
			type    = kCmdLine;
			changed = true;
			}

		if (changed)
			{
			SetIntegration(type, commitEditor, diffCmd, reloadChangedCmd);
			}
		else
			{
			GetCommand(kCommitEditor, &type, &commitEditor);
			setenv("SVN_EDITOR", commitEditor.GetBytes(), 1);
			}
		}
	else
		{
		SetIntegration(hasJCC ? kCodeCrusader : kCmdLine,
					   JString(kCmdLineCmd[ kCommitEditor ], JString::kNoCopy),
					   JString(kCmdLineCmd[ kDiffCmd ], JString::kNoCopy),
					   JString(kCmdLineCmd[ kReloadChangedCmd ], JString::kNoCopy));
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
SVNPrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const bool ok = GetData(kSVNProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 Program state

	RestoreProgramState() returns true if there was any state to restore.

	ForgetProgramState() is required because we have to save state before
	anything is closed.  If the close fails, we shouldn't save the state.

 ******************************************************************************/

bool
SVNPrefsManager::RestoreProgramState()
{
	std::string data;
	if (GetData(kSVNDocMgrStateID, &data))
		{
		std::istringstream dataStream(data);
		const bool restored =
			(SVNGetWDManager())->RestoreState(dataStream);
		RemoveData(kSVNDocMgrStateID);

		return restored;
		}
	else
		{
		return false;
		}
}

void
SVNPrefsManager::SaveProgramState()
{
	std::ostringstream data;
	if ((SVNGetWDManager())->SaveState(data))
		{
		SetData(kSVNDocMgrStateID, data);
		}
	else
		{
		RemoveData(kSVNDocMgrStateID);
		}
}

void
SVNPrefsManager::ForgetProgramState()
{
	RemoveData(kSVNDocMgrStateID);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
SVNPrefsManager::EditPrefs()
{
	assert( itsPrefsDialog == nullptr );

	Integration type;
	JString commitEditor, diffCmd, reloadChangedCmd;
	const bool exists = GetIntegration(&type, &commitEditor, &diffCmd, &reloadChangedCmd);
	assert( exists );

	itsPrefsDialog =
		new SVNPrefsDialog(JXGetApplication(), type, commitEditor, diffCmd, reloadChangedCmd);
	assert( itsPrefsDialog != nullptr );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

bool
SVNPrefsManager::GetWindowSize
	(
	const JPrefID&	id,
	JPoint*			desktopLoc,
	JCoordinate*	width,
	JCoordinate*	height
	)
	const
{
	std::string data;
	if (GetData(id, &data))
		{
		std::istringstream dataStream(data);
		dataStream >> *desktopLoc >> *width >> *height;
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
SVNPrefsManager::SaveWindowSize
	(
	const JPrefID&	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	data << window->GetDesktopLocation();
	data << ' ' << window->GetFrameWidth();
	data << ' ' << window->GetFrameHeight();

	SetData(id, data);
}

/******************************************************************************
 GetCommand (private)

	Returns false if no command is available.

 ******************************************************************************/

bool
SVNPrefsManager::GetCommand
	(
	const Command	cmdType,
	Integration*	type,
	JString*		cmd
	)
	const
{
	JString commitEditor, diffCmd, reloadChangedCmd;
	const bool exists = GetIntegration(type, &commitEditor, &diffCmd, &reloadChangedCmd);
	assert( exists );

	if (*type == kCustom && cmdType == kCommitEditor)
		{
		*cmd = commitEditor;
		}
	else if (*type == kCustom && cmdType == kDiffCmd)
		{
		*cmd = diffCmd;
		}
	else if (*type == kCustom && cmdType == kReloadChangedCmd)
		{
		*cmd = reloadChangedCmd;
		}
	else
		{
		*cmd = kIntegrationCmd[ *type ][ cmdType ];
		}

	return !cmd->IsEmpty();
}

/******************************************************************************
 GetIntegration (private)

 ******************************************************************************/

bool
SVNPrefsManager::GetIntegration
	(
	Integration*	type,
	JString*		commitEditor,
	JString*		diffCmd,
	JString*		reloadChangedCmd
	)
	const
{
	std::string data;
	if (GetData(kSVNIntegrationID, &data))
		{
		std::istringstream dataStream(data);
		dataStream >> *type;
		dataStream >> *commitEditor;
		dataStream >> *diffCmd;
		dataStream >> *reloadChangedCmd;
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 SetIntegration (private)

 ******************************************************************************/

void
SVNPrefsManager::SetIntegration
	(
	const Integration	type,
	const JString&		commitEditor,
	const JString&		diffCmd,
	const JString&		reloadChangedCmd
	)
{
	std::ostringstream data;
	data << type;
	data << ' ' << commitEditor;
	data << ' ' << diffCmd;
	data << ' ' << reloadChangedCmd;

	SetData(kSVNIntegrationID, data);

	Integration t;
	JString e;
	GetCommand(kCommitEditor, &t, &e);
	setenv("SVN_EDITOR", e.GetBytes(), 1);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			Integration type;
			JString commitEditor, diffCmd, reloadChangedCmd;
			itsPrefsDialog->GetData(&type, &commitEditor, &diffCmd, &reloadChangedCmd);

			SetIntegration(type, commitEditor, diffCmd, reloadChangedCmd);
			}
		itsPrefsDialog = nullptr;
		}
	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&						input,
	SVNPrefsManager::Integration&	type
	)
{
	long temp;
	input >> temp;
	type = (SVNPrefsManager::Integration) temp;
	assert( SVNPrefsManager::kCodeCrusader <= type && type <= SVNPrefsManager::kCustom );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&							output,
	const SVNPrefsManager::Integration	type
	)
{
	output << (long) type;
	return output;
}
