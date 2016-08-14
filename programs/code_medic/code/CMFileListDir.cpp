/******************************************************************************
 CMFileListDir.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "CMFileListDir.h"
#include "CMCommandDirector.h"
#include "CMGetSourceFileList.h"
#include "cmHelpText.h"
#include "cmGlobals.h"
#include "cmActionDefs.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXTextButton.h>
#include <JXFileListSet.h>
#include <JXFileListTable.h>
#include <JXHelpManager.h>
#include <JXWDMenu.h>
#include <JXImage.h>
#include <JTableSelection.h>
#include <jAssert.h>

static const JCharacter* kWindowTitleSuffix = " Files";

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

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"    Use wildcard filter %b"
	"  | Use regex filter    %b";

enum
{
	kShowFilterCmd = 1,
	kShowRegexCmd
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

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

CMFileListDir::CMFileListDir
	(
	CMCommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	JPrefObject(CMGetPrefsManager(), kFileListSetupID),
	itsCommandDir(supervisor)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	BuildWindow();

	itsCmd = itsLink->CreateGetSourceFileList(this);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMFileListDir::~CMFileListDir()
{
	JPrefObject::WritePrefs();
	CMGetPrefsManager()->SaveWindowSize(kFileWindSizeID, GetWindow());

	delete itsCmd;
}

/******************************************************************************
 GetTable

 ******************************************************************************/

JXFileListTable*
CMFileListDir::GetTable()
{
	return itsFileListSet->GetTable();
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "medic_file_list_window.xpm"

#include <jx_file_open.xpm>
#include <jx_filter_wildcard.xpm>
#include <jx_filter_regex.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMFileListDir::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 250,500, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 250,30);
	assert( menuBar != NULL );

	itsFileListSet =
		new JXFileListSet(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 250,470);
	assert( itsFileListSet != NULL );

// end JXLayout

	window->SetTitle(kWindowTitleSuffix);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(kJTrue);
	window->SetWMClass(CMGetWMClassInstance(), CMGetFileListWindowClass());
	CMGetPrefsManager()->GetWindowSize(kFileWindSizeID, window);

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, medic_file_list_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	ListenTo(GetTable());

	// menus

	itsFileMenu = menuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr, "CMFileListDir");
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsActionsMenu->SetItemImage(kShowFilterCmd, jx_filter_wildcard);
	itsActionsMenu->SetItemImage(kShowRegexCmd,  jx_filter_regex);

	JXWDMenu* wdMenu =
		new JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != NULL );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMFileListDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMFileListDir::UpdateWindowTitle
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
CMFileListDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMFileListDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMFileListDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetFileListIcon();
	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMFileListDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetTable() && message.Is(JXFileListTable::kProcessSelection))
		{
		JXFileListTable* table          = GetTable();
		const JPtrArray<JString>& files	= table->GetFullNameList();

		JTableSelectionIterator iter(&(table->GetTableSelection()));
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JIndex index = table->RowIndexToFileIndex(cell.y);
			itsCommandDir->OpenSourceFile(*(files.NthElement(index)));
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kSymbolsLoaded))
		{
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
		UpdateWindowTitle(info->GetProgramName());
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
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
CMFileListDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		delete itsCmd;
		itsCmd = itsLink->CreateGetSourceFileList(this);
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMFileListDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMFileListDir::HandleFileMenu
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
 UpdateActionsMenu

 ******************************************************************************/

void
CMFileListDir::UpdateActionsMenu()
{
	const JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
	if (type == JXFileListSet::kWildcardFilter)
		{
		itsActionsMenu->CheckItem(kShowFilterCmd);
		}
	else if (type == JXFileListSet::kRegexFilter)
		{
		itsActionsMenu->CheckItem(kShowRegexCmd);
		}
}

/******************************************************************************
 HandleActionsMenu

 ******************************************************************************/

void
CMFileListDir::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kShowFilterCmd)
		{
		JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
		if (type == JXFileListSet::kWildcardFilter)
			{
			itsFileListSet->SetFilterType(JXFileListSet::kNoFilter);
			}
		else
			{
			itsFileListSet->SetFilterType(JXFileListSet::kWildcardFilter);
			}
		}
	else if (index == kShowRegexCmd)
		{
		JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
		if (type == JXFileListSet::kRegexFilter)
			{
			itsFileListSet->SetFilterType(JXFileListSet::kNoFilter);
			}
		else
			{
			itsFileListSet->SetFilterType(JXFileListSet::kRegexFilter);
			}
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
CMFileListDir::HandleHelpMenu
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
		(JXGetHelpManager())->ShowSection(kCMFileListHelpName);
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

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CMFileListDir::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		itsFileListSet->ReadSetup(input);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CMFileListDir::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	itsFileListSet->WriteSetup(output);
}
