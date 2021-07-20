/******************************************************************************
 CMFileListDir.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "CMFileListDir.h"
#include "CMCommandDirector.h"
#include "CMGetSourceFileList.h"
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

// File menu

static const JUtf8Byte* kFileMenuStr =
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

static const JUtf8Byte* kActionMenuStr =
	"    Use wildcard filter %b"
	"  | Use regex filter    %b";

enum
{
	kShowFilterCmd = 1,
	kShowRegexCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
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

	jdelete itsCmd;
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

	auto* window = jnew JXWindow(this, 250,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 250,30);
	assert( menuBar != nullptr );

	itsFileListSet =
		jnew JXFileListSet(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 250,470);
	assert( itsFileListSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::CMFileListDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(CMGetWMClassInstance(), CMGetFileListWindowClass());
	CMGetPrefsManager()->GetWindowSize(kFileWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_file_list_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	ListenTo(GetTable());

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionsMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::CMGlobal"));
	itsActionsMenu->SetMenuItems(kActionMenuStr, "CMFileListDir");
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsActionsMenu->SetItemImage(kShowFilterCmd, jx_filter_wildcard);
	itsActionsMenu->SetItemImage(kShowRegexCmd,  jx_filter_regex);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
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
	const JString& binaryName
	)
{
	JString title = binaryName;
	title += JGetString("WindowTitleSuffix::CMFileListDir");
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

bool
CMFileListDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetFileListIcon();
	return true;
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
			itsCommandDir->OpenSourceFile(*(files.GetElement(index)));
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kSymbolsLoaded))
		{
		const auto* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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

		jdelete itsCmd;
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
		JXGetApplication()->Quit();
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
		JXGetHelpManager()->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		JXGetHelpManager()->ShowSection("CMOverviewHelp");
		}
	else if (index == kThisWindowCmd)
		{
		JXGetHelpManager()->ShowSection("CMSourceWindowHelp-FileList");
		}
	else if (index == kChangesCmd)
		{
		JXGetHelpManager()->ShowChangeLog();
		}
	else if (index == kCreditsCmd)
		{
		JXGetHelpManager()->ShowCredits();
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CMFileListDir::ReadPrefs
	(
	std::istream& input
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
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	itsFileListSet->WriteSetup(output);
}
