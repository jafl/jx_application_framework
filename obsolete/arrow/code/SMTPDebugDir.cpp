/******************************************************************************
 SMTPDebugDir.cc

	BASE CLASS = GMManagedDirector

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "SMTPDebugDir.h"
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

SMTPDebugDir::SMTPDebugDir
	(
	JXDirector* supervisor
	)
	:
	GMManagedDirector(supervisor)
{
	JSize w = 400;
	JSize h = 300;
	JXWindow* window = new JXWindow(this, w,h, "Debug Window");
    assert( window != NULL );
    SetWindow(window);
    window->SetCloseAction(JXWindow::kDeactivateDirector);

    JXMenuBar* menuBar =
	new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
					0,0, w,kJXStdMenuBarHeight);
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
			0,kJXStdMenuBarHeight,w,h - kJXStdMenuBarHeight);
	assert ( scrollbarSet != NULL );

	itsText =
		new JXStaticText("", kJFalse, kJTrue,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,w,h - kJXStdMenuBarHeight);
	assert (itsText != NULL);

	itsMenuIcon = new JXImage(window->GetDisplay(), window->GetColormap(), JXPM(debug_xpm));
	assert(itsMenuIcon != NULL);
	itsMenuIcon->ConvertToRemoteStorage();

	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SMTPDebugDir::~SMTPDebugDir()
{
	delete itsMenuIcon;
	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
SMTPDebugDir::Receive
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
SMTPDebugDir::AddText
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
SMTPDebugDir::UpdateFileMenu()
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
SMTPDebugDir::HandleFileMenu
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
