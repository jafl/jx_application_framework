/******************************************************************************
 RecordDirector.cpp

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "RecordDirector.h"
#include "RecordTable.h"
#include "HeaderWidget.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Open selected items %k Return. %i" kOpenFilesAction
	"%l| Page setup..."
	"  | Print...            %k Meta-P  %i" kJXPrintAction
	"%l| Close               %k Meta-W  %i" kJXCloseWindowAction
	"%l| Quit                %k Meta-Q  %i" kJXQuitAction;

enum
{
	kOpenSelCmd = 1,
	kPageSetupCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Edit preferences..."
	"  | Edit tool bar..."
	"  | Mac/Win/X emulation..."
	"%l| Save window setup as default";

enum
{
	kPrefsCmd = 1,
	kEditToolBarCmd,
	kEditMacWinPrefsCmd,
	kSaveWindSizeCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

	Our table takes ownership of recordList.

 *****************************************************************************/

RecordDirector::RecordDirector
	(
	JXDirector*		supervisor,
	RecordList*	recordList,
	const JString&	windowTitle
	)
	:
	JXWindowDirector(supervisor),
	itsPrinter(nullptr)
{
	BuildWindow(recordList, windowTitle);

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((GetPrefsManager())->GetWindowSize(kRecordDirectorWindSizeID,
											&desktopLoc, &w, &h))
	{
		JXWindow* window = GetWindow();
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

RecordDirector::~RecordDirector()
{
	jdelete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "md_main_window_icon.xpm"
#include <jx-af/image/jx/jx_help_specific.xpm>
#include <jx-af/image/jx/jx_help_toc.xpm>

void
RecordDirector::BuildWindow
	(
	RecordList*	recordList,
	const JString&	windowTitle
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,400, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kRecordToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,370);
	assert( itsToolBar != nullptr );

// end JXLayout

	window->SetTitle(windowTitle);
	window->SetWMClass(GetWMClassInstance(), GetMainWindowClass());
	window->SetMinSize(200, 200);

	auto* image = jnew JXImage(GetDisplay(), md_main_window_icon);
	assert( image != nullptr );
	window->SetIcon(image);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	const JCoordinate kHeaderHeight	= 25;
	const JCoordinate tableHeight   = scrollbarSet->GetScrollEnclosure()->GetBoundsHeight() - kHeaderHeight;

	itsRecordTable =
		jnew RecordTable(recordList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kHeaderHeight, 100,tableHeight);
	assert( itsRecordTable != nullptr );
	itsRecordTable->FitToEnclosure(true, false);

	auto* tableHeader =
		jnew HeaderWidget(itsRecordTable, recordList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, 100,kHeaderHeight);
	assert( tableHeader != nullptr );
	tableHeader->FitToEnclosure(true, false);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "RecordDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	(itsRecordTable->GetEditMenuHandler())->AppendEditMenu(menuBar);

	auto* windowsMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != nullptr );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "RecordDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "RecordDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);
		itsToolBar->AppendButton(itsFileMenu, kCloseCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
	}

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
RecordDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileMenu();
	}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdatePrefsMenu();
	}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
	{
		 const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateHelpMenu();
	}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
	}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPageSetupFinished))
	{
		const auto* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			(GetPrefsManager())->SavePrintSetup(*itsPrinter);
		}
	}
	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
	{
		const auto* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			(GetPrefsManager())->SavePrintSetup(*itsPrinter);
			itsRecordTable->Print(*itsPrinter);
		}
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
RecordDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kOpenSelCmd, (itsRecordTable->GetTableSelection()).HasSelection());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
RecordDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenSelCmd)
	{
		itsRecordTable->OpenSelectedFiles();
	}

	else if (index == kPageSetupCmd)
	{
		(GetPrefsManager())->LoadPrintSetup(itsPrinter);
		itsPrinter->BeginUserPageSetup();
	}
	else if (index == kPrintCmd)
	{
		(GetPrefsManager())->LoadPrintSetup(itsPrinter);
		itsPrinter->BeginUserPrintSetup();
	}

	else if (index == kCloseCmd)
	{
		Close();
	}
	else if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
RecordDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
RecordDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
	{
		(GetPrefsManager())->EditPrefs();
	}
	else if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		(GetPrefsManager())->SaveWindowSize(kRecordDirectorWindSizeID, GetWindow());
	}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
RecordDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
RecordDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		GetApplication()->DisplayAbout();
	}

	else if (index == kTOCCmd)
	{
		(JXGetHelpManager())->ShowTOC();
	}
	else if (index == kOverviewCmd)
	{
		(JXGetHelpManager())->ShowSection("OverviewHelp");
	}
	else if (index == kThisWindowCmd)
	{
		(JXGetHelpManager())->ShowSection("RecordHelp");
	}

	else if (index == kChangesCmd)
	{
		(JXGetHelpManager())->ShowChangeLog();
	}
	else if (index == kCreditsCmd)
	{
		(JXGetHelpManager())->ShowCredits();
	}
}
