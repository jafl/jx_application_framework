/******************************************************************************
 <PRE>MainDirector.cc

	<Description>

	BASE CLASS = public JXWindowDirector, public JPrefObject

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#include <<pre>StdInc.h>
#include "<PRE>MainDirector.h"
#include "<pre>HelpText.h"
#include "<pre>Globals.h"
#include "<pre>ActionDefs.h"
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

<PRE>MainDirector::<PRE>MainDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(<PRE>GetPrefsManager(), k<PRE>MainDirectorID)
{
	BuildWindow();

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((<PRE>GetPrefsManager())->GetWindowSize(k<PRE>MainDirectorWindSizeID,
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

<PRE>MainDirector::~<PRE>MainDirector()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "<pre>_main_window_icon.xpm"
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
<PRE>MainDirector::BuildWindow()
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
        new JXToolBar(<PRE>GetPrefsManager(), k<PRE>MainToolBarID, menuBar, minWidth, minHeight, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
    assert( itsToolBar != NULL );

// end JXLayout

	window->SetTitle("Program");

	JXImage* image = new JXImage(GetDisplay(), GetColormap(), <pre>_main_window_icon);
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
	itsFileMenu->SetMenuItems(kFileMenuStr, "<PRE>MainDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "<PRE>MainDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "<PRE>MainDirector");
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
<PRE>MainDirector::Receive
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
<PRE>MainDirector::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
<PRE>MainDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kQuitCmd)
		{
		<PRE>GetApplication()->Quit();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
<PRE>MainDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
<PRE>MainDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kPrefsCmd)
		{
		(<PRE>GetPrefsManager())->EditPrefs();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}

	else if (index == kSaveWindSizeCmd)
		{
		(<PRE>GetPrefsManager())->SaveWindowSize(k<PRE>MainDirectorWindSizeID, GetWindow());
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
<PRE>MainDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
<PRE>MainDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		<PRE>GetApplication()->DisplayAbout();
		}

	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(k<PRE>TOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(k<PRE>OverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(k<PRE>MainHelpName);
		}

	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(k<PRE>ChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(k<PRE>CreditsName);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
<PRE>MainDirector::ReadPrefs
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
<PRE>MainDirector::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);
}
