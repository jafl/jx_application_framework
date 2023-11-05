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

#include <jx-af/jcore/JFloatTableData.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// File menu information

static const JUtf8Byte* kFileMenuStr =
	"Page setup... | Print... %h p" "%l| Close %h c";

enum
{
	kPageSetupCmd = 1,
	kPrintCmd,
	kCloseCmd
};

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

void
TestFloatTableDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 320,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

	auto* extraInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,10, 30,20);
	assert( extraInput != nullptr );

	auto* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::TestFloatTableDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,10, 50,20);
	assert( rangeLabel != nullptr );
	rangeLabel->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestFloatTableDirector"));
	window->SetWMClass("testjx", "TestFloatTableDirector");
	window->SetMinSize(150,150);

	extraInput->SetLimits(-5.0, 5.0);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::TestFloatTableDirector"));
	itsFileMenu->SetShortcuts(JGetString("FileMenuShorcut::TestFloatTableDirector"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->AttachHandlers(this,
		&TestFloatTableDirector::UpdateFileMenu,
		&TestFloatTableDirector::HandleFileMenu);

	// layout table and headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

	const JRect tablelayout_Aperture = encl->GetAperture();
	encl->AdjustSize(400 - tablelayout_Aperture.width(), 300 - tablelayout_Aperture.height());

	itsTable =
		jnew TestFloatTable(itsData, menuBar, scrollbarSet, encl,
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
