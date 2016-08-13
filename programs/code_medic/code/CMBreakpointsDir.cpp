/******************************************************************************
 CMBreakpointsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 2010 by John Lindal. All rights reserved.

 *****************************************************************************/

#include "CMBreakpointsDir.h"
#include "CMBreakpointTable.h"
#include "CMBreakpointManager.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include "cmHelpText.h"
#include "cmActionDefs.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <JXWDMenu.h>
#include <JXImage.h>
#include <jAssert.h>

static const JCharacter* kWindowTitleSuffix = " Breakpoints";

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Open source file... %k Meta-O %i" kCMOpenSourceFileAction
	"%l| Close               %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kCloseWindowCmd,
	kQuitCmd
};

// Actions menu

static const JCharacter* kActionMenuTitleStr = "Actions";
static const JCharacter* kActionMenuStr =
	"Remove all breakpoints %k Ctrl-X %i" kCMClearAllBreakpointsAction;

enum
{
	kRemoveAllBreakpointsCmd = 1
};

// Windows menu

static const JCharacter* kWindowsMenuTitleStr = "Windows";

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About"
	"%l| Table of Contents"
	"  | Overview"
	"  | This window %k F1"
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

#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMBreakpointsDir::CMBreakpointsDir
	(
	CMCommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor)
{
	BuildWindow(supervisor);
	ListenTo(CMGetLink());
	ListenTo(CMGetLink()->GetBreakpointManager());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMBreakpointsDir::~CMBreakpointsDir()
{
	(CMGetPrefsManager())->SaveWindowSize(kBreakpointsWindowSizeID, GetWindow());
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "medic_breakpoints_window.xpm"

#include <jx_file_open.xpm>

void
CMBreakpointsDir::BuildWindow
	(
	CMCommandDirector* dir
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 450,500, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle(kWindowTitleSuffix);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(kJTrue);
	window->SetWMClass(CMGetWMClassInstance(), CMGetStackWindowClass());
	(CMGetPrefsManager())->GetWindowSize(kBreakpointsWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, medic_breakpoints_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	// layout table and column headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

	const JRect tablelayout_Frame    = encl->GetFrame();
	const JRect tablelayout_Aperture = encl->GetAperture();
	encl->AdjustSize(400 - tablelayout_Aperture.width(), 300 - tablelayout_Aperture.height());

	itsTable =
		new CMBreakpointTable(this, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 400,280);
	assert( itsTable != NULL );

	itsColHeader =
		new JXColHeaderWidget(itsTable, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,20);
	assert( itsColHeader != NULL );

	encl->SetSize(tablelayout_Frame.width(), tablelayout_Frame.height());

// end tablelayout

	itsTable->SetColTitles(itsColHeader);
	itsColHeader->TurnOnColResizing();

	// menus

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMBreakpointsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(kActionMenuTitleStr);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMBreakpointsDir");
	ListenTo(itsActionMenu);

	JXWDMenu* wdMenu =
		new JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != NULL );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMBreakpointsDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMBreakpointsDir::UpdateWindowTitle
	(
	const JCharacter* binaryName
	)
{
	JString title = binaryName;
	title += kWindowTitleSuffix;
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMBreakpointsDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMBreakpointsDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMBreakpointsDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetBreakpointsIcon();
	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMBreakpointsDir::Receive
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

	else if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionMenu();
		}
	else if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleActionMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == CMGetLink() && message.Is(CMLink::kSymbolsLoaded))
		{
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
		UpdateWindowTitle(info->GetProgramName());
		}

	else if (CMGetLink() != NULL &&
			 sender == CMGetLink()->GetBreakpointManager() &&
			 message.Is(CMBreakpointManager::kBreakpointsChanged))
		{
		itsTable->Update();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMBreakpointsDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!CMIsShuttingDown())
		{
		ListenTo(CMGetLink());
		ListenTo(CMGetLink()->GetBreakpointManager());
		}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMBreakpointsDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMBreakpointsDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
		{
		itsCommandDir->OpenSourceFiles();
		}

	else if (index == kCloseWindowCmd)
		{
		Deactivate();
		}
	else if (index == kQuitCmd)
		{
		(JXGetApplication())->Quit();
		}
}

/******************************************************************************
 UpdateActionMenu

 ******************************************************************************/

void
CMBreakpointsDir::UpdateActionMenu()
{
	CMLink* link = CMGetLink();
	if (link != NULL)
		{
		itsActionMenu->EnableItem(kRemoveAllBreakpointsCmd);
		}
}

/******************************************************************************
 HandleActionMenu

 ******************************************************************************/

void
CMBreakpointsDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kRemoveAllBreakpointsCmd)
		{
		(CMGetLink())->RemoveAllBreakpoints();
		}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
CMBreakpointsDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		(CMGetApplication())->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMBreakpointsHelpName);
		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMCreditsName);
		}
}
