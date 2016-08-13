/******************************************************************************
 TestFloatTableDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestFloatTableDirector.h"
#include "TestFloatTable.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>
#include <JXFloatInput.h>
#include <JXScrollbarSet.h>
#include <JXRowHeaderWidget.h>
#include <JXColHeaderWidget.h>
#include <JXPSPrinter.h>

#include <JFloatTableData.h>
#include <jGlobals.h>
#include <jAssert.h>

// File menu information

static const JCharacter* kFileMenuTitleStr  = "File";
static const JCharacter* kFileMenuShortcuts = "#F";
static const JCharacter* kFileMenuStr =
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
	itsData = new JFloatTableData(0.0);
	assert( itsData != NULL );

	itsPrinter = NULL;

	BuildWindow();

	itsPrinter = new JXPSPrinter(GetDisplay());
	assert( itsPrinter != NULL );
	ListenTo(itsPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFloatTableDirector::~TestFloatTableDirector()
{
	delete itsData;
	delete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestFloatTableDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 400,330, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 320,30);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != NULL );

	JXFloatInput* extraInput =
		new JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,10, 30,20);
	assert( extraInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::TestFloatTableDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,10, 50,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Test Number Table");
	window->SetWMClass("testjx", "TestFloatTableDirector");
	window->SetMinSize(150,150);

	extraInput->SetLimits(-5.0, 5.0);

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetShortcuts(kFileMenuShortcuts);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	ListenTo(itsFileMenu);

	// layout table and headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

	const JRect tablelayout_Frame    = encl->GetFrame();
	const JRect tablelayout_Aperture = encl->GetAperture();
	encl->AdjustSize(400 - tablelayout_Aperture.width(), 300 - tablelayout_Aperture.height());

	itsTable =
		new TestFloatTable(itsData, menuBar, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 390,280);
	assert( itsTable != NULL );

	itsColHeader =
		new JXColHeaderWidget(itsTable, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,0, 390,20);
	assert( itsColHeader != NULL );

	itsRowHeader =
		new JXRowHeaderWidget(itsTable, scrollbarSet, encl,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 0,20, 10,280);
	assert( itsRowHeader != NULL );

	encl->SetSize(tablelayout_Frame.width(), tablelayout_Frame.height());

// end tablelayout

	itsColHeader->TurnOnColResizing();
	ListenTo(itsRowHeader);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TestFloatTableDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsRowHeader && message.Is(JXRowHeaderWidget::kNeedsToBeWidened))
		{
		const JXRowHeaderWidget::NeedsToBeWidened* info =
			dynamic_cast<const JXRowHeaderWidget::NeedsToBeWidened*>(&message);
		assert( info != NULL );

		const JCoordinate dw = info->GetDeltaWidth();
		itsRowHeader->AdjustSize(dw,0);
		itsColHeader->Move(dw,0);
		itsColHeader->AdjustSize(-dw,0);
		itsTable->Move(dw,0);
		itsTable->AdjustSize(-dw,0);
		}

	else if (sender == itsPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsTable->Print(*itsPrinter);
			}
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
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
		itsPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintCmd && itsTable->EndEditing())
		{
		itsPrinter->BeginUserPrintSetup();
		}
	else if (index == kCloseCmd)
		{
		Close();
		}
}
