/******************************************************************************
 PlotDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "PlotDirector.h"
#include <jx-af/j2dplot/JX2DPlotWidget.h>
#include <jx-af/j2dplot/JX2DPlotEPSPrinter.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Actions menu

static const JUtf8Byte* kActionsMenuStr =
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

PlotDirector::PlotDirector
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

PlotDirector::~PlotDirector()
{
	jdelete itsPSPrinter;
	jdelete itsEPSPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PlotDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 600,400, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
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

	ListenTo(itsPlotWidget, std::function([this](const J2DPlotWidget::TitleChanged&)
	{
		GetWindow()->SetTitle(itsPlotWidget->GetTitle());
	}));

	itsActionsMenu = menuBar->PrependTextMenu(JGetString("ActionsMenuTitle::PlotDirector"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsActionsMenu->AttachHandlers(this,
		std::bind(&PlotDirector::UpdateActionsMenu, this),
		std::bind(&PlotDirector::HandleActionsMenu, this, std::placeholders::_1));

	// do this after everything is constructed so Receive() doesn't crash

	itsPlotWidget->SetTitle(JGetString("PlotTitle::PlotDirector"));
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
PlotDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnabled(kPrintMarksEPSCmd, itsPlotWidget->HasMarks());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
PlotDirector::HandleActionsMenu
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
		JXGetApplication()->Quit();
	}
}
