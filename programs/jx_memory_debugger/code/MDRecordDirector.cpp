/******************************************************************************
 MDRecordDirector.cc

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright (C) 2010 by John Lindal. All rights reserved.

 *****************************************************************************/

#include "MDRecordDirector.h"
#include "MDRecordTable.h"
#include "MDHeaderWidget.h"
#include "mdHelpText.h"
#include "mdGlobals.h"
#include "mdActionDefs.h"
#include <JXMacWinPrefsDialog.h>
#include <JXHelpManager.h>
#include <JXWDManager.h>
#include <JXWDMenu.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXPSPrinter.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <JTableSelection.h>
#include <jAssert.h>

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Open selected items %k Return. %i" kMDOpenFilesAction
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

// Windows menu

static const JCharacter* kWindowsMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
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

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
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

MDRecordDirector::MDRecordDirector
	(
	JXDirector*			supervisor,
	MDRecordList*		recordList,
	const JCharacter*	windowTitle
	)
	:
	JXWindowDirector(supervisor),
	itsPrinter(NULL)
{
	BuildWindow(recordList, windowTitle);

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((MDGetPrefsManager())->GetWindowSize(kMDRecordDirectorWindSizeID,
											&desktopLoc, &w, &h))
		{
		JXWindow* window = GetWindow();
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != NULL );
	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDRecordDirector::~MDRecordDirector()
{
	jdelete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "md_main_window_icon.xpm"
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
MDRecordDirector::BuildWindow
	(
	MDRecordList*		recordList,
	const JCharacter*	windowTitle
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,400, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(MDGetPrefsManager(), kMDRecordToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,370);
	assert( itsToolBar != NULL );

// end JXLayout

	window->SetTitle(windowTitle);
	window->SetWMClass(MDGetWMClassInstance(), MDGetMainWindowClass());
	window->SetMinSize(200, 200);

	JXImage* image = jnew JXImage(GetDisplay(), md_main_window_icon);
	assert( image != NULL );
	window->SetIcon(image);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

	const JCoordinate kHeaderHeight	= 25;
	const JCoordinate tableHeight   = scrollbarSet->GetScrollEnclosure()->GetBoundsHeight() - kHeaderHeight;

	itsRecordTable =
		jnew MDRecordTable(recordList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kHeaderHeight, 100,tableHeight);
	assert( itsRecordTable != NULL );
	itsRecordTable->FitToEnclosure(kJTrue, kJFalse);

	MDHeaderWidget* tableHeader =
		jnew MDHeaderWidget(itsRecordTable, recordList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, 100,kHeaderHeight);
	assert( tableHeader != NULL );
	tableHeader->FitToEnclosure(kJTrue, kJFalse);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "MDRecordDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	(itsRecordTable->GetEditMenuHandler())->AppendEditMenu(menuBar);

	JXWDMenu* windowsMenu =
		jnew JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != NULL );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "MDRecordDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "MDRecordDirector");
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
MDRecordDirector::Receive
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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHelpMenu();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPageSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			(MDGetPrefsManager())->SavePrintSetup(*itsPrinter);
			}
		}
	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			(MDGetPrefsManager())->SavePrintSetup(*itsPrinter);
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
MDRecordDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kOpenSelCmd, (itsRecordTable->GetTableSelection()).HasSelection());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MDRecordDirector::HandleFileMenu
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
		(MDGetPrefsManager())->LoadPrintSetup(itsPrinter);
		itsPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintCmd)
		{
		(MDGetPrefsManager())->LoadPrintSetup(itsPrinter);
		itsPrinter->BeginUserPrintSetup();
		}

	else if (index == kCloseCmd)
		{
		Close();
		}
	else if (index == kQuitCmd)
		{
		MDGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
MDRecordDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
MDRecordDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
		{
		(MDGetPrefsManager())->EditPrefs();
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
		(MDGetPrefsManager())->SaveWindowSize(kMDRecordDirectorWindSizeID, GetWindow());
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
MDRecordDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
MDRecordDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		MDGetApplication()->DisplayAbout();
		}

	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDRecordHelpName);
		}

	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kMDCreditsName);
		}
}
