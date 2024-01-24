/******************************************************************************
 DocumentManager.cpp

	BASE CLASS = JXDocumentManager, JPrefObject

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#include "DocumentManager.h"
#include "FileHistoryMenu.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentSetupVersion = 0;

// JBroadcaster message types

const JUtf8Byte* DocumentManager::kAddFileToHistory = "AddFileToHistory::DocumentManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

DocumentManager::DocumentManager()
	:
	JXDocumentManager(),
	JPrefObject(GetPrefsManager(), kDocMgrID),
	itsRecentFileMenu(nullptr)
{
	PrefsManager* prefsMgr = GetPrefsManager();
	ListenTo(prefsMgr);
	// ReadPrefs() called in CreateHistoryMenus()
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DocumentManager::~DocumentManager()
{
	// DeleteGlobals() calls WritePrefs()
}

/******************************************************************************
 CreateFileHistoryMenu

	Called by CreateGlobals() after permanent window has been created.

	*** This must only be called once.

 ******************************************************************************/

void
DocumentManager::CreateFileHistoryMenu
	(
	JXWindow* window
	)
{
	assert( itsRecentFileMenu == nullptr );

	itsRecentFileMenu =
		jnew FileHistoryMenu(window,
							  JXWidget::kFixedLeft, JXWidget::kFixedTop,
							  -20,0, 10,10);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 AddToFileHistoryMenu

 ******************************************************************************/

void
DocumentManager::AddToFileHistoryMenu
	(
	const JString& fullName
	)
{
	Broadcast(AddFileToHistory(fullName));
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
DocumentManager::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	itsRecentFileMenu->ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
DocumentManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	itsRecentFileMenu->WriteSetup(output);
}
