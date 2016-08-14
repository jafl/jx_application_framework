/******************************************************************************
 CMLocalVarsDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CMLocalVarsDir.h"
#include "CMGetLocalVars.h"
#include "CMVarTreeWidget.h"
#include "CMVarNode.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include "cmHelpText.h"
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

static const JCharacter* kWindowTitleSuffix = " Local Variables";

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
	"    Display as 1D array %k Meta-Shift-A %i" kCMDisplay1DArrayAction
	"  | Plot as 1D array                    %i" kCMPlot1DArrayAction
	"  | Display as 2D array                 %i" kCMDisplay2DArrayAction
	"%l| Watch expression                    %i" kCMWatchVarValueAction
	"  | Watch expression location           %i" kCMWatchVarLocationAction
	"%l| Examine memory                      %i" kCMExamineMemoryAction
	"  | Disassemble memory                  %i" kCMDisasmMemoryAction;

enum
{
	kDisplay1DArrayCmd = 1,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd
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
	itsNeedsUpdateFlag(kJFalse)
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

	delete itsGetLocalsCmd;
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
	window->SetWMClass(CMGetWMClassInstance(), CMGetVariableWindowClass());
	CMGetPrefsManager()->GetWindowSize(kLocalVarWindSizeID, window);

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, medic_local_variables_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	CMVarNode* root = itsLink->CreateVarNode(kJFalse);
	assert( root != NULL );
	itsTree = new JTree(root);
	assert( itsTree != NULL );
	JNamedTreeList* treeList = new JNamedTreeList(itsTree);
	assert( treeList != NULL );

	itsWidget =
		new CMVarTreeWidget(itsCommandDir, kJFalse, menuBar, itsTree, treeList,
							scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != NULL);
	itsWidget->FitToEnclosure();

	itsGetLocalsCmd = itsLink->CreateGetLocalVars(root);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionMenu = menuBar->AppendTextMenu(kActionMenuTitleStr);
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMLocalVarsDir");
	ListenTo(itsActionMenu);

	itsActionMenu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	itsActionMenu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	itsActionMenu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	itsActionMenu->SetItemImage(kExamineMemCmd,     medic_show_memory);

	JXWDMenu* wdMenu =
		new JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != NULL );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
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
CMLocalVarsDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMLocalVarsDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMLocalVarsDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetLocalVarsIcon();
	return kJTrue;
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
		const CMLink::ProgramStopped& info =
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

		CMVarNode* root = itsLink->CreateVarNode(kJFalse);
		assert( root != NULL );
		itsTree->SetRoot(root);

		delete itsGetLocalsCmd;
		itsGetLocalsCmd = itsLink->CreateGetLocalVars(root);

		itsNeedsUpdateFlag = kJFalse;
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
		itsNeedsUpdateFlag = kJFalse;	// can't call FlushOldData() since must *update* tree
		itsGetLocalsCmd->Send();

		CMVarNode* root = dynamic_cast<CMVarNode*>(itsTree->GetRoot());
		assert( root != NULL );
		root->SetValid(kJFalse);
		}
	else
		{
		itsNeedsUpdateFlag = kJTrue;
		}
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
CMLocalVarsDir::FlushOldData()
{
	(itsTree->GetRoot())->DeleteAllChildren();
	itsNeedsUpdateFlag = kJFalse;
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
		(JXGetApplication())->Quit();
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
	if (index == kDisplay1DArrayCmd)
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
		(JXGetHelpManager())->ShowSection(kCMTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMLocalVarsHelpName);
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
