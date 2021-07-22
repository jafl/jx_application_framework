/******************************************************************************
 GPMMainDirector.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#include "GPMMainDirector.h"
#include "GPMSystemStats.h"
#include "GPMProcessTable.h"
#include "GPMListHeaderWidget.h"
#include "GPMProcessTreeList.h"
#include "GPMTreeHeaderWidget.h"
#include "GPMProcessList.h"

#include "gpmGlobals.h"
#include "gpmActionDefs.h"

#include <JXApplication.h>
#include <JXHelpManager.h>
#include <JXMenuBar.h>
#include <JXToolBar.h>
#include <JXTabGroup.h>
#include <JXScrollbarSet.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>
#include <JXTimerTask.h>
#include <JXImage.h>
#include <JXColorManager.h>
#include <JXWindow.h>

#include <JNamedTreeList.h>
#include <JSimpleProcess.h>
#include <jAssert.h>

#include "pause.xpm"
#include "cont.xpm"
#include "slow.xpm"
#include "gpm_stop.xpm"
#include "gpm_all_processes.xpm"
#include <jx_edit_clear.xpm>

const JCoordinate kStatusHeight = 30;
const JCoordinate kStatusMargin = 5;

const JFileVersion kCurrentPrefsVersion	= 6;

	// version  6: removed showFullCommand; added tree sorting column
	// version  5: added selected tab index
	// version  4: added list sorting column
	// version  3: added showFullCommand
	// version  2: added window geometry
	// version  1: add showUserOnly

const Time kTimerDelay = 3000;

enum
{
	kListTabIndex = 1,
	kTreeTabIndex
};

// File menu

static const JUtf8Byte* kFileMenuStr =
	"Quit %k Meta-Q %i" kJXQuitAction;

enum
{
	kQuitCmd = 1
};

// Process menu

static const JUtf8Byte* kProcessMenuStr =
	"    Show processes from all users %b %i" kGPMShowAllAction
	"%l| End process                      %i" kGPMStopAction
	"  | Kill process                     %i" kGPMKillAction
	"%l| Pause process                    %i" kGPMPauseAction
	"  | Continue process                 %i" kGPMContinueAction
	"%l| Re-nice process                  %i" kGPMReNiceAction;
//	"  | Send signal to process";

enum
{
	kShowAllCmd = 1,
	kEndCmd,
	kKillCmd,
	kPauseCmd,
	kContinueCmd,
	kReNiceCmd
};

// Prefs menu

static const JUtf8Byte* kPrefsMenuStr =
	"Toolbar buttons...  %i" kGPMToolbarButtonsAction;

enum
{
	kEditToolBarCmd = 1
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

 *****************************************************************************/

GPMMainDirector::GPMMainDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GPMGetPrefsManager(), kGPMMainDirectorID),
	itsTimerTask(nullptr)
{
	itsProcessList	= jnew GPMProcessList();
	assert( itsProcessList != nullptr );

	BuildWindow();

	itsTimerTask = jnew JXTimerTask(kTimerDelay);
	assert( itsTimerTask != nullptr );
	itsTimerTask->Start();
	ListenTo(itsTimerTask);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMMainDirector::~GPMMainDirector()
{
	JPrefObject::WritePrefs();
	jdelete itsProcessList;
	jdelete itsTimerTask;
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "gpm_main_window_icon.xpm"
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
GPMMainDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 530,350, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 530,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GPMGetPrefsManager(), kGPMMainToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 530,300);
	assert( itsToolBar != nullptr );

	itsFullCmdDisplay =
		jnew JXStaticText(JString::empty, false, true, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 0,330, 530,20);
	assert( itsFullCmdDisplay != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GPMMainDirector"));
	window->SetCloseAction(JXWindow::kQuitApp);
	window->SetMinSize(530, 250);
	window->SetWMClass(GPMGetWMClassInstance(), GPMGetMainWindowClass());

	auto* image = jnew JXImage(GetDisplay(), gpm_main_window_icon);
	assert( image != nullptr );
	window->SetIcon(image);

	// system stats

	itsSystemStats =
		jnew GPMSystemStats(itsProcessList, itsToolBar->GetWidgetEnclosure(),
					   JXWidget::kHElastic, JXWidget::kFixedTop,
					   0,kStatusMargin, 100,kStatusHeight);
	assert( itsSystemStats != nullptr );
	itsSystemStats->FitToEnclosure(true, false);

	// tab group

	itsTabGroup =
		jnew JXTabGroup(itsToolBar->GetWidgetEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic,
					   0,0, 100,100);
	assert( itsTabGroup != nullptr );
	itsTabGroup->FitToEnclosure();
	ListenTo(itsTabGroup->GetCardEnclosure());

	const JCoordinate statusHeight = kStatusHeight + 2*kStatusMargin;
	itsTabGroup->AdjustSize(0, -statusHeight);
	itsTabGroup->Move(0, statusHeight);

	JXContainer* listTab = itsTabGroup->AppendTab(JGetString("ListTabTitle::GPMMainDirector"));
	JXContainer* treeTab = itsTabGroup->AppendTab(JGetString("TreeTabTitle::GPMMainDirector"));

	// list view

	auto* scrollbarSet =
		jnew JXScrollbarSet(listTab, JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	const JCoordinate kHeaderHeight	= 25;
	const JCoordinate tableHeight   = scrollbarSet->GetScrollEnclosure()->GetBoundsHeight() - kHeaderHeight;

	itsProcessTable =
		jnew GPMProcessTable(itsProcessList, itsFullCmdDisplay,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kHeaderHeight, 100,tableHeight);
	assert( itsProcessTable != nullptr );
	itsProcessTable->FitToEnclosure(true, false);

	auto* tableHeader =
		jnew GPMListHeaderWidget(itsProcessTable, itsProcessList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, 100,kHeaderHeight);
	assert( tableHeader != nullptr );
	tableHeader->FitToEnclosure(true, false);

	// tree view

	scrollbarSet =
		jnew JXScrollbarSet(treeTab, JXWidget::kHElastic, JXWidget::kVElastic,
						   0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	auto* treeList = jnew JNamedTreeList(itsProcessList->GetProcessTree());
	assert( treeList != nullptr );

	itsProcessTree =
		jnew GPMProcessTreeList(itsProcessList, treeList, itsFullCmdDisplay,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kHeaderHeight, 100,tableHeight);
	assert( itsProcessTree != nullptr );
	itsProcessTree->FitToEnclosure(true, false);

	auto* treeHeader =
		jnew GPMTreeHeaderWidget(itsProcessTree, itsProcessList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, 100,kHeaderHeight);
	assert( treeHeader != nullptr );
	treeHeader->FitToEnclosure(true, false);

	itsProcessTable->SetDefaultRowHeight(itsProcessTree->GetDefaultRowHeight());

	// focus hocus pocus

	window->UnregisterFocusWidget(itsFullCmdDisplay);
	window->RegisterFocusWidget(itsFullCmdDisplay);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsProcessMenu = menuBar->AppendTextMenu(JGetString("ProcessMenuTitle::GPMMainDirector"));
	itsProcessMenu->SetMenuItems(kProcessMenuStr, "GPMProcessTable");
	ListenTo(itsProcessMenu);

	itsProcessMenu->SetItemImage(kShowAllCmd, JXPM(gpm_all_processes));
	itsProcessMenu->SetItemImage(kEndCmd, JXPM(gpm_stop));
	itsProcessMenu->SetItemImage(kKillCmd, JXPM(jx_edit_clear));
	itsProcessMenu->SetItemImage(kPauseCmd, JXPM(gpm_pause));
	itsProcessMenu->SetItemImage(kContinueCmd, JXPM(gpm_cont));
	itsProcessMenu->SetItemImage(kReNiceCmd, JXPM(gpm_slow));

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kQuitCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProcessMenu, kShowAllCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProcessMenu, kEndCmd);
		itsToolBar->AppendButton(itsProcessMenu, kKillCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProcessMenu, kPauseCmd);
		itsToolBar->AppendButton(itsProcessMenu, kContinueCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProcessMenu, kReNiceCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GPMMainDirector::Receive
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

	else if (sender == itsProcessMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateProcessMenu();
		}
	else if (sender == itsProcessMenu && message.Is(JXMenu::kItemSelected))
		{
		 const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleProcessMenu(selection->GetIndex());
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

	else if (sender == itsTimerTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		itsProcessList->Update();
		}

	else if (sender == itsTabGroup->GetCardEnclosure() &&
			 message.Is(JXCardFile::kCardIndexChanged))
		{
		JIndex index;
		const bool ok = itsTabGroup->GetCurrentTabIndex(&index);
		assert( ok );

		const GPMProcessEntry* entry;
		if (index == kListTabIndex && itsProcessTree->GetSelectedProcess(&entry))
			{
			itsProcessTable->SelectProcess(*entry);
			}
		else if (index == kTreeTabIndex && itsProcessTable->GetSelectedProcess(&entry))
			{
			itsProcessTree->SelectProcess(*entry);
			}
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
GPMMainDirector::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
GPMMainDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kQuitCmd)
		{
		GPMGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdateProcessMenu (private)

 ******************************************************************************/

void
GPMMainDirector::UpdateProcessMenu()
{
	itsProcessMenu->EnableItem(kShowAllCmd);
	if (!itsProcessList->WillShowUserOnly())
		{
		itsProcessMenu->CheckItem(kShowAllCmd);
		}

	JIndex tabIndex;
	const bool ok = itsTabGroup->GetCurrentTabIndex(&tabIndex);
	assert( ok );

	const GPMProcessEntry* entry;
	if ((tabIndex == kListTabIndex && itsProcessTable->GetSelectedProcess(&entry)) ||
		(tabIndex == kTreeTabIndex && itsProcessTree->GetSelectedProcess(&entry)))
		{
		if (entry->GetState() != GPMProcessEntry::kZombie)
			{
			const bool notSelf = entry->GetPID() != getpid();
			itsProcessMenu->EnableItem(kEndCmd);
			itsProcessMenu->EnableItem(kKillCmd);
			itsProcessMenu->SetItemEnable(kPauseCmd, notSelf);
			itsProcessMenu->SetItemEnable(kContinueCmd, notSelf);
			itsProcessMenu->EnableItem(kReNiceCmd);
			}
		}
}

/******************************************************************************
 HandleProcessMenu (private)

 ******************************************************************************/

void
GPMMainDirector::HandleProcessMenu
	(
	const JIndex index
	)
{
	if (index == kShowAllCmd)
		{
		itsProcessList->ShouldShowUserOnly(!itsProcessList->WillShowUserOnly());
		return;
		}

	JIndex tabIndex;
	const bool ok = itsTabGroup->GetCurrentTabIndex(&tabIndex);
	assert( ok );

	const GPMProcessEntry* entry;
	if ((tabIndex == kListTabIndex && itsProcessTable->GetSelectedProcess(&entry)) ||
		(tabIndex == kTreeTabIndex && itsProcessTree->GetSelectedProcess(&entry)))
		{
		if (entry->GetState() == GPMProcessEntry::kZombie)
			{
			return;
			}

		if (index == kReNiceCmd)
			{
			JSetProcessPriority(entry->GetPID(), 19);
			return;
			}

		JIndex sigValue = 0;
		if (index == kEndCmd)
			{
			sigValue = SIGTERM;
			}
		else if (index == kKillCmd)
			{
			sigValue = SIGKILL;
			}
		else if (index == kPauseCmd)
			{
			sigValue = SIGSTOP;
			}
		else if (index == kContinueCmd)
			{
			sigValue = SIGCONT;
			}

		const pid_t pid = entry->GetPID();
		if (sigValue == 0 || pid == 0)
			{
			return;
			}

		const uid_t uid = getuid();
		if (uid == 0 || entry->GetUID() == uid)
			{
			JSendSignalToProcess(pid, sigValue);
			itsProcessList->Update();
			}
		else
			{
			JString cmd("xterm -title 'Drakon sudo' -e /bin/sh -c 'sudo -k ; sudo kill -");
			cmd += JString((JUInt64) sigValue);
			cmd += " ";
			cmd += JString((JUInt64) pid);
			cmd += "'";
			JSimpleProcess::Create(cmd, true);
			}
		}
}

/******************************************************************************
 UpdatePrefsMenu

 ******************************************************************************/

void
GPMMainDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
GPMMainDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
}

/******************************************************************************
 UpdateHelpMenu

 ******************************************************************************/

void
GPMMainDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
GPMMainDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		GPMGetApplication()->DisplayAbout();
		}

	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("GPMOverviewHelp");
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection("GPMMainHelp");
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

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
GPMMainDirector::ReadPrefs
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

	if (vers >= 1)
		{
		bool show;
		input >> JBoolFromString(show);
		itsProcessList->ShouldShowUserOnly(show);
		}

	if (vers >= 2)
		{
		GetWindow()->ReadGeometry(input);
		GetWindow()->Deiconify();
		}

	if (3 <= vers && vers < 6)
		{
		bool full;
		input >> JBoolFromString(full);
		}

	if (vers >= 4)
		{
		int type;
		input >> type;
		itsProcessList->ListColSelected(type);
		}

	if (vers >= 6)
		{
		int type;
		input >> type;
		itsProcessList->TreeColSelected(type);
		}

	if (vers >= 5)
		{
		JIndex tabIndex;
		input >> tabIndex;
		itsTabGroup->ShowTab(tabIndex);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
GPMMainDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ' << JBoolToString(itsProcessList->WillShowUserOnly());

	GetWindow()->WriteGeometry(output);

	output << ' ' << (int) itsProcessList->GetSelectedListCol();
	output << ' ' << (int) itsProcessList->GetSelectedTreeCol();

	JIndex tabIndex;
	const bool ok = itsTabGroup->GetCurrentTabIndex(&tabIndex);
	assert( ok );
	output << ' ' << tabIndex;
}
