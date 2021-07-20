/******************************************************************************
 CMArray1DDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMArray1DDir.h"
#include "CMArrayExprInput.h"
#include "CMArrayIndexInput.h"
#include "CMVarTreeWidget.h"
#include "CMVarNode.h"
#include "CMCommandDirector.h"
#include "cmGlobals.h"
#include "cmActionDefs.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXHelpManager.h>
#include <JXWDManager.h>
#include <JXWDMenu.h>
#include <JXImage.h>
#include <JXCloseDirectorTask.h>

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
	"  | Disassemble memory                  %i" kCMDisasmMemoryAction
	"%l| Save window size as default";

enum
{
	kDisplayAsCStringCmd = 1,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd,
	kSavePrefsCmd
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

CMArray1DDir::CMArray1DDir
	(
	CMCommandDirector*	supervisor,
	const JString&		expr
	)
	:
	JXWindowDirector(JXGetApplication())
{
	// format variable for input field

	itsExpr = expr;
	CMVarNode::TrimExpression(&itsExpr);

	itsRequestRange.Set(0, 10);
	CMArray1DDirX(supervisor);
}

CMArray1DDir::CMArray1DDir
	(
	std::istream&			input,
	const JFileVersion	vers,
	CMCommandDirector*	supervisor
	)
	:
	JXWindowDirector(JXGetApplication())
{
	input >> itsExpr >> itsRequestRange;
	CMArray1DDirX(supervisor);
	GetWindow()->ReadGeometry(input);
}

// private

void
CMArray1DDir::CMArray1DDirX
	(
	CMCommandDirector* supervisor
	)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	itsCommandDir           = supervisor;
	itsCurrentNode          = nullptr;
	itsWaitingForReloadFlag = false;
	itsDisplayRange.SetToEmptyAt(0);

	BuildWindow();
	BeginCreateNodes();

	itsCommandDir->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMArray1DDir::~CMArray1DDir()
{
	itsCommandDir->DirectorDeleted(this);
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
CMArray1DDir::StreamOut
	(
	std::ostream& output
	)
{
	output << ' ' << itsExpr;
	output << ' ' << itsRequestRange;
	GetWindow()->WriteGeometry(output);	// must be last
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CMArray1DDir::Activate()
{
	JXWindowDirector::Activate();
	itsWidget->ShouldUpdate(true);
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
CMArray1DDir::Deactivate()
{
	itsWidget->ShouldUpdate(false);
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_1d_array_window.xpm"

#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMArray1DDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 300,500, JString::empty);
	assert( window != nullptr );

	itsExprInput =
		jnew CMArrayExprInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 170,20);
	assert( itsExprInput != nullptr );

	auto* startLabel =
		jnew JXStaticText(JGetString("startLabel::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 90,20);
	assert( startLabel != nullptr );
	startLabel->SetToLabel();

	auto* endLabel =
		jnew JXStaticText(JGetString("endLabel::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 90,20);
	assert( endLabel != nullptr );
	endLabel->SetToLabel();

	itsStopButton =
		jnew JXTextButton(JGetString("itsStopButton::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,85, 60,20);
	assert( itsStopButton != nullptr );
	itsStopButton->SetShortcuts(JGetString("itsStopButton::CMArray1DDir::shortcuts::JXLayout"));

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 300,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,130, 300,370);
	assert( scrollbarSet != nullptr );

	itsStartIndex =
		jnew CMArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,70, 60,20);
	assert( itsStartIndex != nullptr );

	itsEndIndex =
		jnew CMArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,100, 60,20);
	assert( itsEndIndex != nullptr );

	auto* exprLabel =
		jnew JXStaticText(JGetString("exprLabel::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 90,20);
	assert( exprLabel != nullptr );
	exprLabel->SetToLabel();

// end JXLayout

	window->SetMinSize(300, 200);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(CMGetWMClassInstance(), CMGetArray1DWindowClass());

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_1d_array_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	CMGetPrefsManager()->GetWindowSize(kArray1DWindSizeID, window, true);

	CMVarNode* root = itsLink->CreateVarNode();
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

	itsExprInput->GetText()->SetText(itsExpr);
	itsExprInput->SetIsRequired();
	ListenTo(itsExprInput);

	itsStartIndex->SetValue(itsRequestRange.first);
	ListenTo(itsStartIndex);

	itsEndIndex->SetValue(itsRequestRange.last);
	ListenTo(itsEndIndex);

	itsStopButton->Hide();
	ListenTo(itsStopButton);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	JXTEBase* te = itsWidget->GetEditMenuHandler();
	itsExprInput->ShareEditMenu(te);
	itsStartIndex->ShareEditMenu(te);
	itsEndIndex->ShareEditMenu(te);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::CMGlobal"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMArray1DDir");
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
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMArray1DDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMArray1DDir::UpdateWindowTitle()
{
	JString title = itsExpr;
	title += JGetString("WindowTitleSuffix::CMArray1DDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
CMArray1DDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetArray1DIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMArray1DDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsExprInput &&
		(message.Is(JXWidget::kLostFocus) ||
		 message.Is(CMArrayExprInput::kReturnKeyPressed)))
		{
		if (itsExprInput->GetText()->GetText() != itsExpr)
			{
			itsExpr = itsExprInput->GetText()->GetText();
			UpdateWindowTitle();
			itsDisplayRange.SetToEmptyAt(0);
			BeginCreateNodes();
			}
		}
	else if (sender == itsStartIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(CMArrayIndexInput::kReturnKeyPressed)))
		{
		JInteger value;
		if (itsStartIndex->GetValue(&value) && value != itsRequestRange.first)
			{
			itsRequestRange.first = value;
			BeginCreateNodes();
			}
		}
	else if (sender == itsEndIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(CMArrayIndexInput::kReturnKeyPressed)))
		{
		JInteger value;
		if (itsEndIndex->GetValue(&value) && value != itsRequestRange.last)
			{
			itsRequestRange.last = value;
			BeginCreateNodes();
			}
		}

	else if (sender == itsTree && message.Is(JTree::kNodeChanged))
		{
		const auto* info =
			dynamic_cast<const JTree::NodeChanged*>(&message);
		assert(info != nullptr);
		if (info->GetNode() == itsCurrentNode)
			{
			itsCurrentNode = nullptr;
			CreateNextNode();
			}
		}

	else if (sender == itsStopButton && message.Is(JXButton::kPushed))
		{
		itsRequestRange = itsDisplayRange;
		itsStartIndex->SetValue(itsRequestRange.first);
		itsEndIndex->SetValue(itsRequestRange.last);
		CreateNodesFinished();
		}

	else if (sender == itsLink && message.Is(CMLink::kDebuggerRestarted))
		{
		itsWaitingForReloadFlag = true;
		}
	else if (sender == itsLink && message.Is(CMLink::kDebuggerStarted))
		{
		if (!itsWaitingForReloadFlag)
			{
			JXCloseDirectorTask::Close(this);	// close after bcast is finished
			}
		itsWaitingForReloadFlag = false;
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

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMArray1DDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		JXCloseDirectorTask::Close(this);
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 BeginCreateNodes (private)

 ******************************************************************************/

void
CMArray1DDir::BeginCreateNodes()
{
	JIntRange overlap;
	if (JIntersection(itsDisplayRange, itsRequestRange, &overlap))
		{
		JTreeNode* root = itsTree->GetRoot();
		while (itsDisplayRange.first < itsRequestRange.first)
			{
			JTreeNode* node = root->GetChild(1);
			if (node == itsCurrentNode)
				{
				itsCurrentNode = nullptr;
				}
			jdelete node;
			itsDisplayRange.first++;
			}
		while (itsDisplayRange.last > itsRequestRange.last)
			{
			JTreeNode* node = root->GetChild(root->GetChildCount());
			if (node == itsCurrentNode)
				{
				itsCurrentNode = nullptr;
				}
			jdelete node;
			itsDisplayRange.last--;
			}
		}
	else
		{
		(itsTree->GetRoot())->DeleteAllChildren();
		itsCurrentNode = nullptr;
		itsDisplayRange.SetToEmptyAt(itsRequestRange.first);
		}

	if (itsRequestRange.first < itsDisplayRange.first ||
		itsDisplayRange.last  < itsRequestRange.last)
		{
		itsStopButton->Show();
		CreateNextNode();
		}
}

/******************************************************************************
 CreateNextNode (private)

 ******************************************************************************/

void
CMArray1DDir::CreateNextNode()
{
	if (itsCurrentNode != nullptr)
		{
		return;
		}

	JTreeNode* root = itsTree->GetRoot();
	if (itsRequestRange.first < itsDisplayRange.first)
		{
		itsDisplayRange.first--;
		const JString expr = GetExpression(itsDisplayRange.first);
		CMVarNode* node    = itsLink->CreateVarNode(root, expr, expr, JString::empty);
		assert (node != nullptr);

		itsCurrentNode = node;
		root->InsertAtIndex(1, node);	// move it to the top -- inserted by ctor so it will update self
		ListenTo(itsTree);
		}
	else if (itsDisplayRange.last < itsRequestRange.last)
		{
		itsDisplayRange.last++;
		const JString expr = GetExpression(itsDisplayRange.last);
		CMVarNode* node    = itsLink->CreateVarNode(root, expr, expr, JString::empty);
		assert (node != nullptr);

		itsCurrentNode = node;
		ListenTo(itsTree);
		}
	else
		{
		CreateNodesFinished();
		}
}

/******************************************************************************
 GetExpression (private)

 ******************************************************************************/

JString
CMArray1DDir::GetExpression
	(
	const JInteger i
	)
	const
{
	return itsLink->Build1DArrayExpression(itsExpr, i);
}

/******************************************************************************
 CreateNodesFinished (private)

 ******************************************************************************/

void
CMArray1DDir::CreateNodesFinished()
{
	itsCurrentNode = nullptr;
	StopListening(itsTree);

	itsStopButton->Hide();
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMArray1DDir::HandleFileMenu
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
		Close();
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
CMArray1DDir::UpdateActionMenu()
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

	itsActionMenu->EnableItem(kSavePrefsCmd);
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
CMArray1DDir::HandleActionMenu
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

	else if (index == kSavePrefsCmd)
		{
		CMGetPrefsManager()->SaveWindowSize(kArray1DWindSizeID, GetWindow());
		}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
CMArray1DDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("CMVarTreeHelp-Array1D");
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
