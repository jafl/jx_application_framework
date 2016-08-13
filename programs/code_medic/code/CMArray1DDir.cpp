/******************************************************************************
 CMArray1DDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CMArray1DDir.h"
#include "CMArrayExprInput.h"
#include "CMArrayIndexInput.h"
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

static const JCharacter* kWindowTitleSuffix = "  (1D Array)";

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
	"  | Disassemble memory                  %i" kCMDisasmMemoryAction
	"%l| Save window size as default";

enum
{
	kDisplay1DArrayCmd = 1,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd,
	kSavePrefsCmd
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

CMArray1DDir::CMArray1DDir
	(
	CMCommandDirector*	supervisor,
	const JCharacter*	expr
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
	istream&			input,
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
	itsCurrentNode          = NULL;
	itsWaitingForReloadFlag = kJFalse;
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
	ostream& output
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
	itsWidget->ShouldUpdate(kJTrue);
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
CMArray1DDir::Deactivate()
{
	itsWidget->ShouldUpdate(kJFalse);
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

	JXWindow* window = new JXWindow(this, 300,500, "");
	assert( window != NULL );

	itsExprInput =
		new CMArrayExprInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 170,20);
	assert( itsExprInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 90,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 90,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsStopButton =
		new JXTextButton(JGetString("itsStopButton::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,85, 60,20);
	assert( itsStopButton != NULL );
	itsStopButton->SetShortcuts(JGetString("itsStopButton::CMArray1DDir::shortcuts::JXLayout"));

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 300,30);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,130, 300,370);
	assert( scrollbarSet != NULL );

	itsStartIndex =
		new CMArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,70, 60,20);
	assert( itsStartIndex != NULL );

	itsEndIndex =
		new CMArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,100, 60,20);
	assert( itsEndIndex != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::CMArray1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 90,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

// end JXLayout

	window->SetMinSize(300, 200);
	window->ShouldFocusWhenShow(kJTrue);
	window->SetWMClass(CMGetWMClassInstance(), CMGetArray1DWindowClass());

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, medic_1d_array_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	(CMGetPrefsManager())->GetWindowSize(kArray1DWindSizeID, window, kJTrue);

	CMVarNode* root = itsLink->CreateVarNode();
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

	itsExprInput->SetText(itsExpr);
	itsExprInput->SetIsRequired();
	ListenTo(itsExprInput);

	itsStartIndex->SetValue(itsRequestRange.first);
	ListenTo(itsStartIndex);

	itsEndIndex->SetValue(itsRequestRange.last);
	ListenTo(itsEndIndex);

	itsStopButton->Hide();
	ListenTo(itsStopButton);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	JXTEBase* te = itsWidget->GetEditMenuHandler();
	itsExprInput->ShareEditMenu(te);
	itsStartIndex->ShareEditMenu(te);
	itsEndIndex->ShareEditMenu(te);

	itsActionMenu = menuBar->AppendTextMenu(kActionMenuTitleStr);
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMArray1DDir");
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
	title += kWindowTitleSuffix;
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMArray1DDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetArray1DIcon();
	return kJTrue;
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
		if (itsExprInput->GetText() != itsExpr)
			{
			itsExpr = itsExprInput->GetText();
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
		const JTree::NodeChanged* info =
			dynamic_cast<const JTree::NodeChanged*>(&message);
		assert(info != NULL);
		if (info->GetNode() == itsCurrentNode)
			{
			itsCurrentNode = NULL;
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
		itsWaitingForReloadFlag = kJTrue;
		}
	else if (sender == itsLink && message.Is(CMLink::kDebuggerStarted))
		{
		if (!itsWaitingForReloadFlag)
			{
			JXCloseDirectorTask::Close(this);	// close after bcast is finished
			}
		itsWaitingForReloadFlag = kJFalse;
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
				itsCurrentNode = NULL;
				}
			delete node;
			itsDisplayRange.first++;
			}
		while (itsDisplayRange.last > itsRequestRange.last)
			{
			JTreeNode* node = root->GetChild(root->GetChildCount());
			if (node == itsCurrentNode)
				{
				itsCurrentNode = NULL;
				}
			delete node;
			itsDisplayRange.last--;
			}
		}
	else
		{
		(itsTree->GetRoot())->DeleteAllChildren();
		itsCurrentNode = NULL;
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
	if (itsCurrentNode != NULL)
		{
		return;
		}

	JTreeNode* root = itsTree->GetRoot();
	if (itsRequestRange.first < itsDisplayRange.first)
		{
		itsDisplayRange.first--;
		const JString expr = GetExpression(itsDisplayRange.first);
		CMVarNode* node    = itsLink->CreateVarNode(root, expr, expr, "");
		assert (node != NULL);

		itsCurrentNode = node;
		root->InsertAtIndex(1, node);	// move it to the top -- inserted by ctor so it will update self
		ListenTo(itsTree);
		}
	else if (itsDisplayRange.last < itsRequestRange.last)
		{
		itsDisplayRange.last++;
		const JString expr = GetExpression(itsDisplayRange.last);
		CMVarNode* node    = itsLink->CreateVarNode(root, expr, expr, "");
		assert (node != NULL);

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
	itsCurrentNode = NULL;
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
		(JXGetApplication())->Quit();
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

	else if (index == kSavePrefsCmd)
		{
		(CMGetPrefsManager())->SaveWindowSize(kArray1DWindSizeID, GetWindow());
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
		(JXGetHelpManager())->ShowSection(kCMTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kCMArray1DHelpName);
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
