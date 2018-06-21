/******************************************************************************
 MDPrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDPrefsManager.h"
#include "MDPrefsDialog.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXChooseSaveFile.h>
#include <JXPSPrinter.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion = 0;

static const JString kDefaultOpenFileCmd("jcc $f:$l", kJFalse);

/******************************************************************************
 Constructor

 *****************************************************************************/

MDPrefsManager::MDPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue),
	itsPrefsDialog(nullptr)
{
	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kMDgCSFSetupID);
	csf->JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDPrefsManager::~MDPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
MDPrefsManager::SaveAllBeforeDestruct()
{
	SetData(kMDProgramVersionID, MDGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
MDPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		SetData(kMDProgramVersionID, MDGetVersionNumberStr());
		}
}

/******************************************************************************
 GetPrevVersionStr

 ******************************************************************************/

JString
MDPrefsManager::GetPrevVersionStr()
	const
{
	std::string data;
	const JBoolean ok = GetData(kMDProgramVersionID, &data);
	assert( ok );
	return JString(data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
MDPrefsManager::EditPrefs()
{
	assert( itsPrefsDialog == nullptr );

	const JString openCmd = GetOpenFileCommand();

	itsPrefsDialog = jnew MDPrefsDialog(JXGetApplication(), openCmd);
	assert( itsPrefsDialog != nullptr );
	ListenTo(itsPrefsDialog);
	itsPrefsDialog->BeginDialog();
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

JBoolean
MDPrefsManager::GetWindowSize
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
MDPrefsManager::SaveWindowSize
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

JBoolean
MDPrefsManager::LoadPrintSetup
	(
	JXPSPrinter* p
	)
	const
{
	std::string data;
	if (GetData(kMDPrintSetupID, &data))
		{
		std::istringstream dataStream(data);
		p->ReadXPSSetup(dataStream);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SavePrintSetup

 ******************************************************************************/

void
MDPrefsManager::SavePrintSetup
	(
	const JXPSPrinter& p
	)
{
	std::ostringstream data;
	p.WriteXPSSetup(data);

	SetData(kMDPrintSetupID, data);
}

/******************************************************************************
 GetOpenFileCommand

 ******************************************************************************/

JString
MDPrefsManager::GetOpenFileCommand()
	const
{
	JString cmd;

	std::string data;
	if (GetData(kMDOpenCmdID, &data))
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
MDPrefsManager::SetOpenFileCommand
	(
	const JString& cmd
	)
{
	std::ostringstream data;
	data << cmd;

	SetData(kMDOpenCmdID, data);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MDPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
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
