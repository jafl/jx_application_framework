/******************************************************************************
 MDMainDirector.cc

	<Description>

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright © 2008 by New Planet Software. All rights reserved.

 *****************************************************************************/

#include <mdStdInc.h>
#include "MDMainDirector.h"
#include "mdHelpText.h"
#include "mdGlobals.h"
#include "mdActionDefs.h"
#include <JXHelpManager.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <jAssert.h>

// menus

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"Quit %k Meta-Q %i" kJXQuitAction;

enum
{
	kQuitCmd = 1
};

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Edit preferences..."
	"  | Edit tool bar..."
	"%l| Save window setup as default";

enum
{
	kPrefsCmd = 1,
	kEditToolBarCmd,
	kSaveWindSizeCmd
};

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

// preferences

const JFileVersion kCurrentPrefsVersion	= 0;

// string ID's

/******************************************************************************
 Constructor

 *****************************************************************************/

MDMainDirector::MDMainDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(MDGetPrefsManager(), kMDMainDirectorID)
{
	BuildWindow();

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((MDGetPrefsManager())->GetWindowSize(kMDMainDirectorWindSizeID,
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

MDMainDirector::~MDMainDirector()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "md_main_window_icon.xpm"
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
MDMainDirector::BuildWindow()
{
	const JCoordinate minWidth  = 200;
	const JCoordinate minHeight = 200;

// begin JXLayout

    JXWindow* window = new JXWindow(this, 500,300, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
    assert( menuBar != NULL );

    itsToolBar =
        new JXToolBar(MDGetPrefsManager(), kMDMainToolBarID, menuBar, minWidth, minHeight, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
    assert( itsToolBar != NULL );

// end JXLayout

	window->SetTitle("Program");

	JXImage* image = new JXImage(GetDisplay(), GetColormap(), md_main_window_icon);
	assert( image != NULL );
	window->SetIcon(image);

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

    JXStaticText* widget =
		new JXStaticText("This should be replaced by your widget.",
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
    assert( widget != NULL );
	widget->FitToEnclosure();
    widget->SetBackColor(GetColormap()->GetWhiteColor());
    widget->SetBorderWidth(2);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "MDMainDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "MDMainDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "MDMainDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	// must be done after creating widgets

	JPrefObject::ReadPrefs();

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kQuitCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MDMainDirector::Receive
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
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
MDMainDirector::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MDMainDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kQuitCmd)
		{
		MDGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
MDMainDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
MDMainDirector::HandlePrefsMenu
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

	else if (index == kSaveWindSizeCmd)
		{
		(MDGetPrefsManager())->SaveWindowSize(kMDMainDirectorWindSizeID, GetWindow());
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
MDMainDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
MDMainDirector::HandleHelpMenu
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
		(JXGetHelpManager())->ShowSection(kMDMainHelpName);
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

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
MDMainDirector::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentPrefsVersion)
		{
		(GetWindow())->ReadGeometry(input);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
MDMainDirector::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);
}
