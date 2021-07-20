/******************************************************************************
 CMMemoryDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "CMMemoryDir.h"
#include "CMGetMemory.h"
#include "CMArrayExprInput.h"
#include "CMArrayIndexInput.h"
#include "CMCommandDirector.h"
#include "CMVarNode.h"
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
#include <JXFontManager.h>

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
	"Save window size as default";

enum
{
	kSavePrefsCmd = 1
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

// Display type

static const JUtf8Byte* kDisplayTypeMenuStr =
	"    Hex bytes"
	"  | Hex short (2 bytes)"
	"  | Hex word (4 bytes)"
	"  | Hex long (8 bytes)"
	"%l| Characters (ISO8859-1)"
	"%l| Assembly code";

/******************************************************************************
 Constructor

 *****************************************************************************/

CMMemoryDir::CMMemoryDir
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

	itsDisplayType = kHexByte;
	itsItemCount   = 64;

	CMMemoryDirX(supervisor);
}

CMMemoryDir::CMMemoryDir
	(
	std::istream&		input,
	const JFileVersion	vers,
	CMCommandDirector*	supervisor
	)
	:
	JXWindowDirector(JXGetApplication())
{
	long type;
	input >> itsExpr >> type >> itsItemCount;
	itsDisplayType = (DisplayType) type;

	CMMemoryDirX(supervisor);

	GetWindow()->ReadGeometry(input);
}

// private

void
CMMemoryDir::CMMemoryDirX
	(
	CMCommandDirector* supervisor
	)
{
	itsLink = CMGetLink();
	UpdateDisplayTypeMenu();
	ListenTo(itsLink);

	itsCommandDir           = supervisor;
	itsShouldUpdateFlag     = false;		// window is always initially hidden
	itsNeedsUpdateFlag      = !itsExpr.IsEmpty();
	itsWaitingForReloadFlag = false;

	BuildWindow();

	itsCmd = itsLink->CreateGetMemory(this);

	itsCommandDir->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMMemoryDir::~CMMemoryDir()
{
	itsCommandDir->DirectorDeleted(this);
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
CMMemoryDir::StreamOut
	(
	std::ostream& output
	)
{
	output << ' ' << itsExpr;
	output << ' ' << (long) itsDisplayType;
	output << ' ' << itsItemCount;
	GetWindow()->WriteGeometry(output);	// must be last
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CMMemoryDir::Activate()
{
	JXWindowDirector::Activate();
	itsShouldUpdateFlag = true;
	Update();
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
CMMemoryDir::Deactivate()
{
	itsShouldUpdateFlag = false;
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_memory_window.xpm"

#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMMemoryDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	auto* exprLabel =
		jnew JXStaticText(JGetString("exprLabel::CMMemoryDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 80,20);
	assert( exprLabel != nullptr );
	exprLabel->SetToLabel();

	itsExprInput =
		jnew CMArrayExprInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,40, 380,20);
	assert( itsExprInput != nullptr );

	itsDisplayTypeMenu =
		jnew JXTextMenu(JGetString("itsDisplayTypeMenu::CMMemoryDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 400,25);
	assert( itsDisplayTypeMenu != nullptr );

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::CMMemoryDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,105, 80,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsItemCountInput =
		jnew CMArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,105, 60,20);
	assert( itsItemCountInput != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,135, 500,365);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetMinSize(300, 200);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(CMGetWMClassInstance(), CMGetMemoryWindowClass());

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_memory_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	CMGetPrefsManager()->GetWindowSize(kMemoryWindSizeID, window, true);

	itsWidget =
		jnew JXStaticText(JString::empty, false, true, true,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != nullptr);
	itsWidget->FitToEnclosure();

	JString name;
	JSize size;
	CMGetPrefsManager()->GetDefaultFont(&name, &size);
	itsWidget->SetFont(JFontManager::GetFont(name, size));

	itsExprInput->GetText()->SetText(itsExpr);
	itsExprInput->SetIsRequired();
	ListenTo(itsExprInput);

	itsDisplayTypeMenu->SetMenuItems(kDisplayTypeMenuStr);
	itsDisplayTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDisplayTypeMenu->SetToPopupChoice(true, itsDisplayType);
	ListenTo(itsDisplayTypeMenu);

	itsItemCountInput->SetLowerLimit(1);
	itsItemCountInput->SetValue(itsItemCount);
	ListenTo(itsItemCountInput);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsWidget->AppendEditMenu(menuBar);
	itsExprInput->ShareEditMenu(itsWidget);
	itsItemCountInput->ShareEditMenu(itsWidget);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::CMGlobal"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "CMMemoryDir");
	ListenTo(itsActionMenu);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMMemoryDir");
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
CMMemoryDir::UpdateWindowTitle()
{
	JString title = itsExpr;
	title += JGetString("WindowTitleSuffix::CMMemoryDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
CMMemoryDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetMemoryIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMMemoryDir::Receive
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
			itsNeedsUpdateFlag = true;
			Update();
			}
		}
	else if (sender == itsDisplayTypeMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		if (selection->GetIndex() != (JIndex) itsDisplayType)
			{
			itsDisplayType = (DisplayType) selection->GetIndex();

			// update all other values, too, since menu selection doesn't trigger input fields

			itsExpr = itsExprInput->GetText()->GetText();

			JInteger value;
			if (itsItemCountInput->GetValue(&value))
				{
				itsItemCount = value;
				}

			itsNeedsUpdateFlag = true;
			Update();
			}
		}
	else if (sender == itsItemCountInput &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(CMArrayIndexInput::kReturnKeyPressed)))
		{
		JInteger value;
		if (itsItemCountInput->GetValue(&value) && JSize(value) != itsItemCount)
			{
			itsItemCount       = value;
			itsNeedsUpdateFlag = true;
			Update();
			}
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
	else if (sender == itsLink &&
			 (message.Is(CMLink::kProgramFinished) ||
			  message.Is(CMLink::kCoreCleared)     ||
			  message.Is(CMLink::kDetachedFromProcess)))
		{
		Update(JString::empty);
		}
	else if (sender == itsLink && CMVarNode::ShouldUpdate(message))
		{
		itsNeedsUpdateFlag = true;
		Update();
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
CMMemoryDir::ReceiveGoingAway
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
 UpdateDisplayTypeMenu (private)

 ******************************************************************************/

void
CMMemoryDir::UpdateDisplayTypeMenu()
{
	if (!itsLink->GetFeature(CMLink::kExamineMemory))
		{
		itsDisplayTypeMenu->DisableItem(kHexByte);
		itsDisplayTypeMenu->DisableItem(kHexShort);
		itsDisplayTypeMenu->DisableItem(kHexWord);
		itsDisplayTypeMenu->DisableItem(kHexLong);
		itsDisplayTypeMenu->DisableItem(kChar);
		}

	if (!itsLink->GetFeature(CMLink::kDisassembleMemory))
		{
		itsDisplayTypeMenu->DisableItem(kAsm);
		}
}

/******************************************************************************
 SetDisplayType

 ******************************************************************************/

void
CMMemoryDir::SetDisplayType
	(
	const DisplayType type
	)
{
	if (itsDisplayTypeMenu->IsEnabled(type))
		{
		itsDisplayType = type;
		itsDisplayTypeMenu->SetPopupChoice(type);
		}
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
CMMemoryDir::Update()
{
	if (itsShouldUpdateFlag && itsNeedsUpdateFlag)
		{
		itsNeedsUpdateFlag = false;

		if (itsCmd != nullptr)
			{
			itsCmd->CMCommand::Send();
			}
		}
}

/******************************************************************************
 Update

 ******************************************************************************/

void
CMMemoryDir::Update
	(
	const JString& data
	)
{
	itsWidget->GetText()->SetText(data);
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMMemoryDir::HandleFileMenu
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
CMMemoryDir::UpdateActionMenu()
{
	itsActionMenu->EnableItem(kSavePrefsCmd);
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
CMMemoryDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kSavePrefsCmd)
		{
		CMGetPrefsManager()->SaveWindowSize(kMemoryWindSizeID, GetWindow());
		}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
CMMemoryDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("CMVarTreeHelp-Memory");
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
