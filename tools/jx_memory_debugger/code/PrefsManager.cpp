/******************************************************************************
 PrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "PrefsManager.h"
#include "PrefsDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXChooseSaveFile.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

static const JString kDefaultOpenFileCmd("jcc $f:$l", JString::kNoCopy);

/******************************************************************************
 Constructor

 *****************************************************************************/

PrefsManager::PrefsManager
	(
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true),
	itsPrefsDialog(nullptr)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PrefsManager::~PrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
PrefsManager::SaveAllBeforeDestruct()
{
	SetData(kProgramVersionID, GetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
PrefsManager::UpgradeData
	(
	const bool		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
	{
		SetData(kProgramVersionID, GetVersionNumberStr());
	}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
PrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const bool ok = GetData(kProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
PrefsManager::EditPrefs()
{
	assert( itsPrefsDialog == nullptr );

	const JString openCmd = GetOpenFileCommand();

	itsPrefsDialog = jnew PrefsDialog(JXGetApplication(), openCmd);
	assert( itsPrefsDialog != nullptr );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

bool
PrefsManager::GetWindowSize
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
PrefsManager::SaveWindowSize
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
 LoadPrintSetup

 ******************************************************************************/

bool
PrefsManager::LoadPrintSetup
	(
	JXPSPrinter* p
	)
	const
{
	std::string data;
	if (GetData(kPrintSetupID, &data))
	{
		std::istringstream dataStream(data);
		p->ReadXPSSetup(dataStream);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SavePrintSetup

 ******************************************************************************/

void
PrefsManager::SavePrintSetup
	(
	const JXPSPrinter& p
	)
{
	std::ostringstream data;
	p.WriteXPSSetup(data);

	SetData(kPrintSetupID, data);
}

/******************************************************************************
 GetOpenFileCommand

 ******************************************************************************/

JString
PrefsManager::GetOpenFileCommand()
	const
{
	JString cmd;

	std::string data;
	if (GetData(kOpenCmdID, &data))
	{
		std::istringstream dataStream(data);
		dataStream >> cmd;
	}
	else
	{
		cmd = kDefaultOpenFileCmd;
	}

	return cmd;
}

/******************************************************************************
 SetOpenFileCommand

 ******************************************************************************/

void
PrefsManager::SetOpenFileCommand
	(
	const JString& cmd
	)
{
	std::ostringstream data;
	data << cmd;

	SetData(kOpenCmdID, data);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
PrefsManager::Receive
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
			JString openCmd;
			itsPrefsDialog->GetValues(&openCmd);

			SetOpenFileCommand(openCmd);
		}
		itsPrefsDialog = nullptr;
	}

	else
	{
		JXPrefsManager::Receive(sender, message);
	}
}
