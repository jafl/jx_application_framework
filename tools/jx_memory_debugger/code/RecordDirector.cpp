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
	if (GetPrefsManager()->GetWindowSize(kRecordDirectorWindSizeID,
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
#include "RecordDirector-File.h"
#include "All-Preferences.h"

void
RecordDirector::BuildWindow
	(
	RecordList*	recordList,
	const JString&	windowTitle
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,400, JString::empty);

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

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::RecordDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&RecordDirector::UpdateFileMenu,
		&RecordDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsRecordTable->GetEditMenuHandler()->AppendEditMenu(menuBar);

	auto* windowsMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != nullptr );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::All_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &RecordDirector::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "RecordHelp");

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
RecordDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kOpenSelCmd, (itsRecordTable->GetTableSelection()).HasSelection());
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
		GetPrefsManager()->LoadPrintSetup(itsPrinter);
		if (itsPrinter->EditUserPageSetup())
		{
			GetPrefsManager()->SavePrintSetup(*itsPrinter);
		}
	}
	else if (index == kPrintCmd)
	{
		GetPrefsManager()->LoadPrintSetup(itsPrinter);
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			GetPrefsManager()->SavePrintSetup(*itsPrinter);
			itsRecordTable->Print(*itsPrinter);
		}
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
