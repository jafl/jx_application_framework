/******************************************************************************
 CMRegistersDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "CMRegistersDir.h"
#include "CMGetRegisters.h"
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

CMRegistersDir::CMRegistersDir
	(
	CMCommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsCommandDir(supervisor),
	itsShouldUpdateFlag(false),	// window is always initially hidden
	itsNeedsUpdateFlag(true)
{
	itsCmd = CMGetLink()->CreateGetRegisters(this);

	BuildWindow();
	ListenTo(CMGetLink());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMRegistersDir::~CMRegistersDir()
{
	CMGetPrefsManager()->SaveWindowSize(kRegistersWindowSizeID, GetWindow());

	jdelete itsCmd;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CMRegistersDir::Activate()
{
	JXWindowDirector::Activate();
	itsShouldUpdateFlag = true;
	Update();
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
CMRegistersDir::Deactivate()
{
	itsShouldUpdateFlag = false;
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_registers_window.xpm"

#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMRegistersDir::BuildWindow()
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

	window->SetTitle(JGetString("WindowTitleSuffix::CMRegistersDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(CMGetWMClassInstance(), CMGetRegistersWindowClass());
	CMGetPrefsManager()->GetWindowSize(kRegistersWindowSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_registers_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

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

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsWidget->AppendEditMenu(menuBar);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMRegistersDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMRegistersDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title        += JGetString("WindowTitleSuffix::CMRegistersDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMRegistersDir::GetName()
	const
{
	return JGetString("WindowsMenuText::CMRegistersDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
CMRegistersDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetRegistersIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMRegistersDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CMGetLink() &&
		(message.Is(CMLink::kProgramFinished) ||
		 message.Is(CMLink::kCoreCleared)     ||
		 message.Is(CMLink::kDetachedFromProcess)))
		{
		Update(JString::empty);
		}
	else if (sender == CMGetLink() &&
			 (message.Is(CMLink::kProgramStopped) ||
			  CMVarNode::ShouldUpdate(message)))
		{
		itsNeedsUpdateFlag = true;
		Update();
		}

	else if (sender == CMGetLink() && message.Is(CMLink::kSymbolsLoaded))
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
CMRegistersDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!CMIsShuttingDown())
		{
		ListenTo(CMGetLink());

		Update(JString::empty);

		jdelete itsCmd;
		itsCmd = CMGetLink()->CreateGetRegisters(this);
		}

	JXWindowDirector::ReceiveGoingAway(sender);
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
CMRegistersDir::Update()
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
CMRegistersDir::Update
	(
	const JString& data
	)
{
	JXTEBase::DisplayState state = itsWidget->SaveDisplayState();
	itsWidget->GetText()->SetText(data);
	itsWidget->RestoreDisplayState(state);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMRegistersDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CMRegistersDir::HandleFileMenu
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
 HandleHelpMenu (private)

 ******************************************************************************/

void
CMRegistersDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("CMVarTreeHelp-Registers");
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
