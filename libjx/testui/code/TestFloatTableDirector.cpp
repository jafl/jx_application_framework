/******************************************************************************
 TestFloatTableDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestFloatTableDirector.h"
#include "TestFloatTable.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXRowHeaderWidget.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/JFloatTableData.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFloatTableDirector::TestFloatTableDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsData = jnew JFloatTableData(0.0);

	itsPrinter = nullptr;

	BuildWindow();

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFloatTableDirector::~TestFloatTableDirector()
{
	jdelete itsData;
	jdelete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "TestAnyTableDirector-File.h"

void
TestFloatTableDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JGetString("WindowTitle::TestFloatTableDirector::JXLayout"));
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "TestFloatTableDirector");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 320,30);
	assert( menuBar != nullptr );

	auto* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::TestFloatTableDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,10, 50,20);
	rangeLabel->SetToLabel(false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

	itsTable =
		jnew TestFloatTable(itsData, menuBar, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 390,280);

	itsColHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,0, 390,20);

	itsRowHeader =
		jnew JXRowHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kVElastic, 0,20, 10,280);

	auto* extraInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,10, 30,20);
	extraInput->SetIsRequired(false);

// end JXLayout

	itsColHeader->TurnOnColResizing();
	ListenTo(itsRowHeader, std::function([this](const JXRowHeaderWidget::NeedsToBeWidened& msg)
	{
		const JCoordinate dw = msg.GetDeltaWidth();
		itsRowHeader->AdjustSize(dw,0);
		itsColHeader->Move(dw,0);
		itsColHeader->AdjustSize(-dw,0);
		itsTable->Move(dw,0);
		itsTable->AdjustSize(-dw,0);
	}));

	extraInput->SetLimits(-5.0, 5.0);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TestAnyTableDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->AttachHandlers(this,
		&TestFloatTableDirector::UpdateFileMenu,
		&TestFloatTableDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TestFloatTableDirector::UpdateFileMenu()
{
	itsFileMenu->EnableItem(kPageSetupCmd);
	itsFileMenu->EnableItem(kCloseCmd);

	if (!itsData->IsEmpty())
	{
		itsFileMenu->EnableItem(kPrintCmd);
	}
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
TestFloatTableDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kPageSetupCmd)
	{
		itsPrinter->EditUserPageSetup();
	}
	else if (index == kPrintCmd && itsTable->EndEditing())
	{
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			itsTable->Print(*itsPrinter);
		}
	}
	else if (index == kCloseCmd)
	{
		Close();
	}
}
