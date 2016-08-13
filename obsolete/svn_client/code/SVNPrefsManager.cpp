/******************************************************************************
 SVNPrefsManager.cc

	BASE CLASS = public JXPrefsManager

	Copyright © 2008 by John Lindal. All rights reserved.

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

static const JCharacter* kCodeCrusaderCmd[] =
{
	"jcc --vcs-commit",
	"",		// not used
	"jcc --reload-open"
};

static const JCharacter* kCmdLineCmd[] =
{
	"gnome-terminal -t \"Commit\" -x vi",
	"gnome-terminal -t \"svn diff $rev_option $file_name\" -x /bin/sh -c \"svn diff $rev_option $file_name | less\"",
	""		// not possible
};

static const JCharacter** kIntegrationCmd[2] =
{
	kCodeCrusaderCmd,
	kCmdLineCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

SVNPrefsManager::SVNPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue),
	itsPrefsDialog(NULL)
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

JBoolean
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
		return JI2B(!input.fail());
		}
	else
		{
		return kJFalse;
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
	const JBoolean		isNew,
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

			const JCharacter* editor = getenv("SVN_EDITOR");
			if (type == kCodeCrusader)
				{
				commitEditor = kCodeCrusaderCmd[ kCommitEditor ];
				}
			else if (type == kCustom && !JStringEmpty(editor))
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

	const JBoolean hasJCC = JProgramAvailable("jcc");

	Integration type;
	JString commitEditor, diffCmd, reloadChangedCmd;
	if (GetIntegration(&type, &commitEditor, &diffCmd, &reloadChangedCmd))
		{
		JBoolean changed = kJFalse;
		if (hasJCC && type != kCodeCrusader)
			{
			type    = kCodeCrusader;
			changed = kJTrue;
			}
		else if (!hasJCC && type == kCodeCrusader)
			{
			type    = kCmdLine;
			changed = kJTrue;
			}

		if (changed)
			{
			SetIntegration(type, commitEditor, diffCmd, reloadChangedCmd);
			}
		else
			{
			GetCommand(kCommitEditor, &type, &commitEditor);
			setenv("SVN_EDITOR", commitEditor, 1);
			}
		}
	else
		{
		SetIntegration(hasJCC ? kCodeCrusader : kCmdLine,
					   kCmdLineCmd[ kCommitEditor ],
					   kCmdLineCmd[ kDiffCmd ],
					   kCmdLineCmd[ kReloadChangedCmd ]);
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
	const JBoolean ok = GetData(kSVNProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 Program state

	RestoreProgramState() returns kJTrue if there was any state to restore.

	ForgetProgramState() is required because we have to save state before
	anything is closed.  If the close fails, we shouldn't save the state.

 ******************************************************************************/

JBoolean
SVNPrefsManager::RestoreProgramState()
{
	std::string data;
	if (GetData(kSVNDocMgrStateID, &data))
		{
		std::istringstream dataStream(data);
		const JBoolean restored =
			(SVNGetWDManager())->RestoreState(dataStream);
		RemoveData(kSVNDocMgrStateID);

		return restored;
		}
	else
		{
		return kJFalse;
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
	assert( itsPrefsDialog == NULL );

	Integration type;
	JString commitEditor, diffCmd, reloadChangedCmd;
	const JBoolean exists = GetIntegration(&type, &commitEditor, &diffCmd, &reloadChangedCmd);
	assert( exists );

	itsPrefsDialog =
		new SVNPrefsDialog(JXGetApplication(), type, commitEditor, diffCmd, reloadChangedCmd);
	assert( itsPrefsDialog != NULL );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

JBoolean
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
		return kJTrue;
		}
	else
		{
		return kJFalse;
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

	Returns kJFalse if no command is available.

 ******************************************************************************/

JBoolean
SVNPrefsManager::GetCommand
	(
	const Command	cmdType,
	Integration*	type,
	JString*		cmd
	)
	const
{
	JString commitEditor, diffCmd, reloadChangedCmd;
	const JBoolean exists = GetIntegration(type, &commitEditor, &diffCmd, &reloadChangedCmd);
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

	return !JStringEmpty(*cmd);
}

/******************************************************************************
 GetIntegration (private)

 ******************************************************************************/

JBoolean
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
		return kJTrue;
		}
	else
		{
		return kJFalse;
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
	setenv("SVN_EDITOR", e, 1);
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
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			Integration type;
			JString commitEditor, diffCmd, reloadChangedCmd;
			itsPrefsDialog->GetData(&type, &commitEditor, &diffCmd, &reloadChangedCmd);

			SetIntegration(type, commitEditor, diffCmd, reloadChangedCmd);
			}
		itsPrefsDialog = NULL;
		}
	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

istream&
operator>>
	(
	istream&						input,
	SVNPrefsManager::Integration&	type
	)
{
	long temp;
	input >> temp;
	type = (SVNPrefsManager::Integration) temp;
	assert( SVNPrefsManager::kCodeCrusader <= type && type <= SVNPrefsManager::kCustom );
	return input;
}

ostream&
operator<<
	(
	ostream&							output,
	const SVNPrefsManager::Integration	type
	)
{
	output << (long) type;
	return output;
}
