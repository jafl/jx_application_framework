/******************************************************************************
 CMLocalVarsDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMLocalVarsDir.h"
#include "CMGetLocalVars.h"
#include "CMVarTreeWidget.h"
#include "CMVarNode.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include "cmActionDefs.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXInputField.h>
#include <JXTextButton.h>
#include <JXHelpManager.h>
#include <JXWDMenu.h>
#include <JXImage.h>

#include <JTree.h>
#include <JNamedTreeList.h>
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
	"    Display as C string %k Meta-S       %i" kCMDisplayAsCStringAction
	"%l| Display as 1D array %k Meta-Shift-A %i" kCMDisplay1DArrayAction
	"  | Plot as 1D array                    %i" kCMPlot1DArrayAction
	"  | Display as 2D array                 %i" kCMDisplay2DArrayAction
	"%l| Watch expression                    %i" kCMWatchVarValueAction
	"  | Watch expression location           %i" kCMWatchVarLocationAction
	"%l| Examine memory                      %i" kCMExamineMemoryAction
	"  | Disassemble memory                  %i" kCMDisasmMemoryAction;

enum
{
	kDisplayAsCStringCmd = 1,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd
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

/******************************************************************************
 Constructor

 *****************************************************************************/

CMLocalVarsDir::CMLocalVarsDir
	(
	CMCommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor),
	itsNeedsUpdateFlag(false)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	BuildWindow();
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMLocalVarsDir::~CMLocalVarsDir()
{
	CMGetPrefsManager()->SaveWindowSize(kLocalVarWindSizeID, GetWindow());

	jdelete itsGetLocalsCmd;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CMLocalVarsDir::Activate()
{
	JXWindowDirector::Activate();
	Update();
}

/******************************************************************************
 BuildWindow (private)

 *****************************************************************************/

#include "medic_local_variables_window.xpm"

#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMLocalVarsDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 450,470);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::CMLocalVarsDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(CMGetWMClassInstance(), CMGetLocalVariableWindowClass());
	CMGetPrefsManager()->GetWindowSize(kLocalVarWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_local_variables_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	CMVarNode* root = itsLink->CreateVarNode(false);
	assert( root != nullptr );
	itsTree = jnew JTree(root);
	assert( itsTree != nullptr );
	auto* treeList = jnew JNamedTreeList(itsTree);
	assert( treeList != nullptr );

	itsWidget =
		jnew CMVarTreeWidget(itsCommandDir, false, menuBar, itsTree, treeList,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != nullptr);
	itsWidget->FitToEnclosure();

	itsGetLocalsCmd = itsLink->CreateGetLocalVars(root);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::CMGlobal"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMLocalVarsDir");
	ListenTo(itsActionMenu);

	itsActionMenu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	itsActionMenu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	itsActionMenu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	itsActionMenu->SetItemImage(kExamineMemCmd,     medic_show_memory);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMLocalVarsDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMLocalVarsDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title += JGetString("WindowTitleSuffix::CMLocalVarsDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMLocalVarsDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMLocalVarsDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
CMLocalVarsDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetLocalVarsIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMLocalVarsDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink &&
		(message.Is(CMLink::kValueChanged)   ||
		 message.Is(CMLink::kThreadChanged)  ||
		 message.Is(CMLink::kFrameChanged)   ||
		 message.Is(CMLink::kCoreLoaded)     ||
		 message.Is(CMLink::kAttachedToProcess)))
		{
		Rebuild();
		}
	else if (sender == itsLink && message.Is(CMLink::kProgramStopped))
		{
		const auto& info =
			dynamic_cast<const CMLink::ProgramStopped&>(message);

		const CMLocation* loc;
		if (info.GetLocation(&loc) && !(loc->GetFileName()).IsEmpty())
			{
			Rebuild();
			}
		}
	else if (sender == itsLink &&
			 (message.Is(CMLink::kProgramFinished) ||
			  message.Is(CMLink::kCoreCleared)     ||
			  message.Is(CMLink::kDetachedFromProcess)))
		{
		// can't listen for CMLink::kProgramRunning because this happens
		// every time the user executes another line of code

		FlushOldData();
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

	else if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionMenu();
		}
	else if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		Update();
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
CMLocalVarsDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		(itsTree->GetRoot())->DeleteAllChildren();

		CMVarNode* root = itsLink->CreateVarNode(false);
		assert( root != nullptr );
		itsTree->SetRoot(root);

		jdelete itsGetLocalsCmd;
		itsGetLocalsCmd = itsLink->CreateGetLocalVars(root);

		itsNeedsUpdateFlag = false;
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
CMLocalVarsDir::Update()
{
	if ((itsLink->HasCore() || itsLink->ProgramIsStopped()) &&
		itsNeedsUpdateFlag)
		{
		Rebuild();
		}
}

/******************************************************************************
 Rebuild (private)

 ******************************************************************************/

void
CMLocalVarsDir::Rebuild()
{
	if (IsActive() && !GetWindow()->IsIconified())
		{
		itsNeedsUpdateFlag = false;	// can't call FlushOldData() since must *update* tree
		itsGetLocalsCmd->Send();

		auto* root = dynamic_cast<CMVarNode*>(itsTree->GetRoot());
		assert( root != nullptr );
		root->SetValid(false);
		}
	else
		{
		itsNeedsUpdateFlag = true;
		}
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
CMLocalVarsDir::FlushOldData()
{
	(itsTree->GetRoot())->DeleteAllChildren();
	itsNeedsUpdateFlag = false;
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMLocalVarsDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMLocalVarsDir::HandleFileMenu
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
 UpdateActionMenu

 ******************************************************************************/

void
CMLocalVarsDir::UpdateActionMenu()
{
	if (itsWidget->HasSelection())
		{
		itsActionMenu->EnableItem(kDisplayAsCStringCmd);
		itsActionMenu->EnableItem(kDisplay1DArrayCmd);
		itsActionMenu->EnableItem(kPlot1DArrayCmd);
		itsActionMenu->EnableItem(kDisplay2DArrayCmd);
		itsActionMenu->EnableItem(kWatchVarCmd);
		itsActionMenu->EnableItem(kWatchLocCmd);
		}

	if (itsLink->GetFeature(CMLink::kExamineMemory))
		{
		itsActionMenu->EnableItem(kExamineMemCmd);
		}
	if (itsLink->GetFeature(CMLink::kDisassembleMemory))
		{
		itsActionMenu->EnableItem(kDisassembleMemCmd);
		}
}

/******************************************************************************
 HandleActionMenu

 ******************************************************************************/

void
CMLocalVarsDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kDisplayAsCStringCmd)
		{
		itsWidget->DisplayAsCString();
		}

	else if (index == kDisplay1DArrayCmd)
		{
		itsWidget->Display1DArray();
		}
	else if (index == kPlot1DArrayCmd)
		{
		itsWidget->Plot1DArray();
		}
	else if (index == kDisplay2DArrayCmd)
		{
		itsWidget->Display2DArray();
		}

	else if (index == kWatchVarCmd)
		{
		itsWidget->WatchExpression();
		}
	else if (index == kWatchLocCmd)
		{
		itsWidget->WatchLocation();
		}

	else if (index == kExamineMemCmd)
		{
		itsWidget->ExamineMemory(CMMemoryDir::kHexByte);
		}
	else if (index == kDisassembleMemCmd)
		{
		itsWidget->ExamineMemory(CMMemoryDir::kAsm);
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
CMLocalVarsDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("CMVarTreeHelp-Local");
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
