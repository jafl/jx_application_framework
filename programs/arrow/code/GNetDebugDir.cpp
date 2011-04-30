/******************************************************************************
 GNetDebugDir.cc

	BASE CLASS = GMManagedDirector

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GNetDebugDir.h"
#include "GMDirectorManager.h"
#include "GMDirectorMenu.h"
#include "debug.xpm"

#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXImage.h>

#include <GMGlobals.h>
#include <jAssert.h>

static const JCharacter* kWindowsMenuTitleStr = "Windows";

static const JCharacter* kFileMenuTitleStr = "Actions";
static const JCharacter* kFileMenuStr =
	"Copy %k Meta-C"
	"|Close %k Meta-W";

enum
{
	kCopyCmd = 1,
	kCloseCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

GNetDebugDir::GNetDebugDir
	(
	JXDirector*		supervisor,
	const JCharacter*	title
	)
	:
	GMManagedDirector(supervisor)
{
	JSize w = 400;
	JSize h = 300;
	JXWindow* window = new JXWindow(this, w,h, title);
    assert( window != NULL );

    window->SetCloseAction(JXWindow::kDeactivateDirector);

    JXMenuBar* menuBar =
	new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
					0,0, w,kJXDefaultMenuBarHeight);
    assert( menuBar != NULL );

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	GMDirectorMenu* menu =
		new GMDirectorMenu(kWindowsMenuTitleStr, menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert(menu != NULL);
	menuBar->AppendMenu(menu);

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight,w,h - kJXDefaultMenuBarHeight);
	assert ( scrollbarSet != NULL );

	itsText =
		new JXStaticText("", kJFalse, kJTrue,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,w,h - kJXDefaultMenuBarHeight);
	assert (itsText != NULL);

	itsMenuIcon = new JXImage(window->GetDisplay(), debug_xpm);
	assert(itsMenuIcon != NULL);
	itsMenuIcon->ConvertToRemoteStorage();

	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GNetDebugDir::~GNetDebugDir()
{
	delete itsMenuIcon;
	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GNetDebugDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}

	GMManagedDirector::Receive(sender, message);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GNetDebugDir::AddText
	(
	const JCharacter* text
	)
{
	JSize count = itsText->GetTextLength();
	itsText->SetCaretLocation(count);
	itsText->Paste("\n");
	itsText->Paste(text);
}

/******************************************************************************
 UpdateFileMenu


 ******************************************************************************/

void
GNetDebugDir::UpdateFileMenu()
{
/*	if (itsText->HasSelection())
		{
		itsFileMenu->EnableItem(kCopyCmd);
		}
	else
		{
		itsFileMenu->DisableItem(kCopyCmd);
		}*/
}

/******************************************************************************
 HandleFileMenu


 ******************************************************************************/

void
GNetDebugDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kCopyCmd)
		{
		if (!itsText->HasSelection())
			{
			itsText->SelectAll();
			}
		itsText->Copy();
		}
	else if (index == kCloseCmd)
		{
		Deactivate();
		}
}
