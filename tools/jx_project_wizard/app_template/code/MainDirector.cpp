/******************************************************************************
 MainDirector.cpp

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright (C) <Year> by <Company>.

 *****************************************************************************/

#include "MainDirector.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

// preferences

const JFileVersion kCurrentPrefsVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

MainDirector::MainDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kMainDirectorID)
{
	BuildWindow();

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kMainDirectorWindSizeID,
										 &desktopLoc, &w, &h))
	{
		JXWindow* window = GetWindow();
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MainDirector::~MainDirector()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "main_window_icon.xpm"
#include "MainDirector-File.h"
#include "MainDirector-Preferences.h"

void
MainDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JGetString("WindowTitle::MainDirector::JXLayout"));
	window->SetMinSize(200, 200);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "<Binary>_Main_Window");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kMainToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 500,270);
	assert( scrollbarSet != nullptr );

	auto* widget =
		jnew JXStaticText(JGetString("widget::MainDirector::JXLayout"), scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 500,270);
	widget->SetToLabel(true);

// end JXLayout

	JXImage* image = jnew JXImage(GetDisplay(), main_window_icon);
	window->SetIcon(image);

	widget->SetBackColor(JColorManager::GetWhiteColor());
	widget->SetBorderWidth(2);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::MainDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&MainDirector::UpdateFileMenu,
		&MainDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::MainDirector_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &MainDirector::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "MainHelp");

	// must be done after creating widgets

	JPrefObject::ReadPrefs();

	itsToolBar->LoadPrefs(nullptr);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kQuitCmd);
		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
MainDirector::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MainDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
MainDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
	{
		GetPrefsManager()->EditPrefs();
	}
	else if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditFileWebPrefsCmd)
	{
		JXGetWebBrowser()->EditPrefs();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		GetPrefsManager()->SaveWindowSize(kMainDirectorWindSizeID, GetWindow());
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
MainDirector::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
	{
		return;
	}

	GetWindow()->ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
MainDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);
}
