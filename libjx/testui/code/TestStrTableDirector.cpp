/******************************************************************************
 TestStrTableDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestStrTableDirector.h"
#include "TestStringTable.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXRowHeaderWidget.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXPSPrinter.h>

#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestStrTableDirector::TestStrTableDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsData = jnew JStringTableData;

	itsPrinter = nullptr;

	BuildWindow();

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestStrTableDirector::~TestStrTableDirector()
{
	jdelete itsData;
	jdelete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "TestAnyTableDirector-File.h"

void
TestStrTableDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestStrTableDirector"));
	window->SetWMClass("testjx", "TestStrTableDirector");
	window->SetMinSize(150,150);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TestAnyTableDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->AttachHandlers(this,
		&TestStrTableDirector::UpdateFileMenu,
		&TestStrTableDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	// layout table and headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

	const JRect tablelayout_Aperture = encl->GetAperture();
	encl->AdjustSize(400 - tablelayout_Aperture.width(), 300 - tablelayout_Aperture.height());

	itsTable =
		jnew TestStringTable(itsData, menuBar, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 390,280);
	assert( itsTable != nullptr );

	itsColHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,0, 390,20);
	assert( itsColHeader != nullptr );

	itsRowHeader =
		jnew JXRowHeaderWidget(itsTable, scrollbarSet, encl,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 0,20, 10,280);
	assert( itsRowHeader != nullptr );

	encl->SetSize(tablelayout_Aperture.width(), tablelayout_Aperture.height());

// end tablelayout

	itsTable->TurnOnRowResizing(itsRowHeader);
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
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TestStrTableDirector::UpdateFileMenu()
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
TestStrTableDirector::HandleFileMenu
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
