/******************************************************************************
 TestStrTableDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

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
#include <jGlobals.h>
#include <jAssert.h>

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

TestStrTableDirector::TestStrTableDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsData = jnew JStringTableData;
	assert( itsData != nullptr );

	itsPrinter = nullptr;

	BuildWindow();

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
	ListenTo(itsPrinter);
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

void
TestStrTableDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != nullptr );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestStrTableDirector"));
	window->SetWMClass("testjx", "TestStrTableDirector");
	window->SetMinSize(150,150);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::TestStrTableDirector"));
	itsFileMenu->SetShortcuts(JGetString("FileMenuShortcut::TestStrTableDirector"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	ListenTo(itsFileMenu);

	// layout table and headers

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

// begin tablelayout

	const JRect tablelayout_Frame    = encl->GetFrame();
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

	encl->SetSize(tablelayout_Frame.width(), tablelayout_Frame.height());

// end tablelayout

	itsTable->TurnOnRowResizing(itsRowHeader);
	itsColHeader->TurnOnColResizing();
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
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsRowHeader && message.Is(JXRowHeaderWidget::kNeedsToBeWidened))
		{
		const JXRowHeaderWidget::NeedsToBeWidened* info =
			dynamic_cast<const JXRowHeaderWidget::NeedsToBeWidened*>(&message);
		assert( info != nullptr );

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
		assert( info != nullptr );
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
