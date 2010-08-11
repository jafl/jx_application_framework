/******************************************************************************
 SCVarListDirector.cpp

	Class to display a variable list associated with a Circuit.
	Our supervisor owns the CircuitVarList object.

	BASE CLASS = JXWindowDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCVarListDirector.h"
#include "SCAddFnDialog.h"
#include "SCVarTable.h"

#include "SCCircuitVarList.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>

#include <JFunction.h>
#include <JString.h>
#include <JTableSelection.h>
#include <jAssert.h>

// Variable menu

static const JCharacter* kVarMenuTitleStr = "Variable";
static const JCharacter* kVarMenuStr =
	"New function... %k Meta-N | Edit function... %k Meta-E"
	"%l| Hide %k Meta-W";

enum
{
	kNewFnCmd = 1,
	kEditFnCmd,
	kHideWindowCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SCVarListDirector::SCVarListDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsVarList = NULL;
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCVarListDirector::~SCVarListDirector()
{
}

/******************************************************************************
 SetVarList

 ******************************************************************************/

void
SCVarListDirector::SetVarList
	(
	SCCircuitVarList* varList
	)
{
	itsVarList = varList;
	itsVarTable->SetVarList(varList);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SCVarListDirector::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 260,280, "");
    assert( window != NULL );

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 260,30);
    assert( menuBar != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 260,250);
    assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle("Extra variables");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(200,100);

	itsVarMenu = menuBar->AppendTextMenu(kVarMenuTitleStr);
	itsVarMenu->SetMenuItems(kVarMenuStr);
	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsVarMenu);

	// create column header and table

	const JCoordinate hdrH = 20;

	JXContainer* encl   = scrollbarSet->GetScrollEnclosure();
	const JCoordinate w = encl->GetBoundsWidth();
	const JCoordinate h = encl->GetBoundsHeight();

	itsVarTable =
		new SCVarTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic,
					   0,hdrH, w,h-hdrH);
	assert( itsVarTable != NULL );

	JXColHeaderWidget* colHeader =
		JXColHeaderWidget::Create(itsVarTable, scrollbarSet,
							  scrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, w,hdrH);
	assert( colHeader != NULL );

	itsVarTable->ConfigureColHeader(colHeader);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCVarListDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsVarMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateVarMenu();
		}
	else if (sender == itsVarMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleVarMenu(selection->GetIndex());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateVarMenu (private)

 ******************************************************************************/

void
SCVarListDirector::UpdateVarMenu()
{
	JTableSelection& s = itsVarTable->GetTableSelection();
	itsVarMenu->SetItemEnable(kEditFnCmd, s.HasSelection());
}

/******************************************************************************
 HandleVarMenu (private)

 ******************************************************************************/

void
SCVarListDirector::HandleVarMenu
	(
	const JIndex index
	)
{
	if (index == kNewFnCmd && itsVarList != NULL)
		{
		JXDialogDirector* dialog = new SCAddFnDialog(this, itsVarList);
		assert( dialog != NULL );
		ListenTo(dialog);
		dialog->BeginDialog();
		}
	else if (index == kEditFnCmd && itsVarList != NULL)
		{
		itsVarTable->EditFunction();
		}
	else if (index == kHideWindowCmd)
		{
		Deactivate();
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SCVarListDirector::ReadSetup
	(
	istream&			input,
	const JFileVersion	vers
	)
{
	(GetWindow())->ReadGeometry(input);

	JBoolean active;
	input >> active;

	itsVarTable->ReadSetup(input, vers);

	if (active)
		{
		Activate();
		}
	else
		{
		Deactivate();
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
SCVarListDirector::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ';
	(GetWindow())->WriteGeometry(output);
	output << ' ' << IsActive();
	output << ' ';
	itsVarTable->WriteSetup(output);
	output << ' ';
}
