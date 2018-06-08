/******************************************************************************
 Test2DPlotDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "Test2DPlotDirector.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JX2DPlotWidget.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <jXGlobals.h>
#include <jAssert.h>

// Actions menu

static const JCharacter* kActionsMenuStr =
	"    PostScript page setup..."
	"  | Print PostScript...      %k Meta-P"
	"%l| Print plot as EPS..."
	"  | Print marks as EPS..."
	"%l| Quit                     %k Meta-Q";

enum
{
	kPSPageSetupCmd = 1, kPrintPSCmd,
	kPrintPlotEPSCmd, kPrintMarksEPSCmd,
	kQuitCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

Test2DPlotDirector::Test2DPlotDirector
	(
	JXDirector*	supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Test2DPlotDirector::~Test2DPlotDirector()
{
	jdelete itsPSPrinter;
	jdelete itsEPSPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
Test2DPlotDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 600,400, JString::empty);
	assert( window != nullptr );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != nullptr );

	itsPlotWidget =
		jnew JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,370);
	assert( itsPlotWidget != nullptr );

// end JXLayout

	window->SetMinSize(300, 200);

	itsPSPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPSPrinter != nullptr );
	itsPlotWidget->SetPSPrinter(itsPSPrinter);

	itsEPSPrinter = jnew JX2DPlotEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != nullptr );
	itsPlotWidget->SetEPSPrinter(itsEPSPrinter);

	ListenTo(itsPlotWidget);

	itsActionsMenu = menuBar->PrependTextMenu(JGetString("ActionsMenuTitle::Test2DPlotDirector"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	// do this after everything is constructed so Receive() doesn't crash

	itsPlotWidget->SetTitle("New plot");
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Test2DPlotDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlotWidget && message.Is(J2DPlotWidget::kTitleChanged))
		{
		GetWindow()->SetTitle(itsPlotWidget->GetTitle());
		}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
Test2DPlotDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnable(kPrintMarksEPSCmd, itsPlotWidget->HasMarks());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
Test2DPlotDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kPSPageSetupCmd)
		{
		itsPlotWidget->HandlePSPageSetup();
		}
	else if (index == kPrintPSCmd)
		{
		itsPlotWidget->PrintPS();
		}

	else if (index == kPrintPlotEPSCmd)
		{
		itsPlotWidget->PrintPlotEPS();
		}
	else if (index == kPrintMarksEPSCmd)
		{
		itsPlotWidget->PrintMarksEPS();
		}

	else if (index == kQuitCmd)
		{
		(JXGetApplication())->Quit();
		}
}
