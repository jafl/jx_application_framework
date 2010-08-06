/******************************************************************************
 TestStrTableDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestStrTableDirector.h"
#include "TestStringTable.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXRowHeaderWidget.h>
#include <JXColHeaderWidget.h>
#include <JXPSPrinter.h>

#include <JStringTableData.h>
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

TestStrTableDirector::TestStrTableDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsData = new JStringTableData;
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

TestStrTableDirector::~TestStrTableDirector()
{
	delete itsData;
	delete itsPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestStrTableDirector::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 400,330, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
    assert( menuBar != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
    assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle("Test String Table");
	window->SetWMClass("testjx", "TestStrTableDirector");
	window->SetMinSize(150,150);

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
        new TestStringTable(itsData, menuBar, scrollbarSet, encl,
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

	itsTable->TurnOnRowResizing(itsRowHeader);
	itsColHeader->TurnOnColResizing(20);
	ListenTo(itsRowHeader);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TestStrTableDirector::Receive
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
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsRowHeader && message.Is(JXRowHeaderWidget::kNeedsToBeWidened))
		{
		const JXRowHeaderWidget::NeedsToBeWidened* info =
			dynamic_cast(const JXRowHeaderWidget::NeedsToBeWidened*, &message);
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
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
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
